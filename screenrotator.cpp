#include "screenrotator.h"
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QProcess>
#include <KPluginFactory>

#include <QDebug>

#include <KConfigCore/KConfigGroup>
#include <KConfigCore/KSharedConfig>

#include <QLoggingCategory>

const QString ScreenRotator::NAME = "kded_rotation";
const QString ScreenRotator::CONFIG = NAME + "rc";
const QString ScreenRotator::ENABLED = "enabled";
const QString ScreenRotator::LOCKED = "locked";
const QString ScreenRotator::DELAY_DURATION = "delay-duration";

const int ScreenRotator::DEFAULT_DELAY_DURATION = 10;

Q_DECLARE_LOGGING_CATEGORY(L)

Q_LOGGING_CATEGORY(L, "k_rotation", QtDebugMsg)

ScreenRotator::ScreenRotator(QObject *parent, const QVariantList &) : KDEDModule(parent) {
  qInfo(L) << "Init" << NAME << "KDED module";

  refreshConfig();

  sensor = new QOrientationSensor(this);

  qInfo(L).nospace() << "State: duration=" << getDelayDuration() << " enabled=" << isEnabled() << " locked=" << isLocked();

  sensor->start();

  connect(sensor, &QOrientationSensor::readingChanged, this, &ScreenRotator::startProgress);
  connect(&timer, &QTimer::timeout, this, &ScreenRotator::updateProgress);

  updateOrientation();

  progress = -1;
}

void ScreenRotator::startProgress() {
  refreshConfig();
  if(!isEnabled() || isLocked()) {
    qDebug(L).nospace() << "Skip rotation, enabled=" << isEnabled() << "locked=" << isLocked();
    return;
  }
  int duration = getDelayDuration();
  duration = duration < 1 || duration > DEFAULT_DELAY_DURATION * 20
      ? DEFAULT_DELAY_DURATION
      : duration;

  qDebug(L).nospace() << "Arm " << NAME << " for rotation, delay=" << duration;
  if (progress == -1) {
    timer.start(duration);
    progress = 0;
  }
}

void ScreenRotator::updateProgress() {
  if (!sensor->reading()) {
    return;
  }
  if (sensor->reading()->orientation() != currentOrientation) {
    progress++;

    QDBusMessage msg = QDBusMessage::createMethodCall(
          QStringLiteral("org.kde.plasmashell"),
          QStringLiteral("/org/kde/osdService"),
          QStringLiteral("org.kde.osdService"),
          QStringLiteral("mediaPlayerVolumeChanged")
          );

    msg.setArguments({progress, "screen", "view-refresh"});

    QDBusConnection::sessionBus().asyncCall(msg);

    if (progress == 100) {
      updateOrientation();
      timer.stop();
      progress = -1;
    }
  } else {
    qDebug(L) << "Dismiss rotation";
    timer.stop();
    progress = -1;
  }
}

void ScreenRotator::updateOrientation() {
  if (!sensor->reading()) {
    return;
  }
  currentOrientation = sensor->reading()->orientation();
  QString o;
  switch (currentOrientation) {
  case QOrientationReading::TopUp:
    o = "normal";
    break;
  case QOrientationReading::TopDown:
    o = "bottom-up";
    break;
  case QOrientationReading::LeftUp:
    o = "left-up";
    break;
  case QOrientationReading::RightUp:
    o = "right-up";
    break;
  default:
    o = "none";
    return;
  }

  qDebug(L) << "Rotate to" << o << "screen and pens";
  helper.start("orientation-helper", {"-r", o});
}

ScreenRotator::~ScreenRotator() {
  delete sensor;
}

void ScreenRotator::refreshConfig() {
  KSharedConfig::Ptr config = KSharedConfig::openConfig(CONFIG);
  config->reparseConfiguration();
  group = config->group(QStringLiteral("General"));
}

bool ScreenRotator::isEnabled() {
  return group.readEntry(ENABLED, true);
}

bool ScreenRotator::isLocked() {
  return group.readEntry(LOCKED, false);
}

int ScreenRotator::getDelayDuration() {
  return group.readEntry(DELAY_DURATION, DEFAULT_DELAY_DURATION);
}


K_PLUGIN_FACTORY(ScreenRotatorFactory,
                 registerPlugin<ScreenRotator>();)
#include "screenrotator.moc"
