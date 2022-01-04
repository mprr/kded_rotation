#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QOrientationSensor>
#include <QTimer>
#include <QProcess>
#include <KDEDModule>
#include <KPluginFactory>

#include <KConfigCore/KConfigGroup>
#include <KConfigCore/KSharedConfig>

class ScreenRotator : public KDEDModule {

  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.kde.rotation")

public:
  ScreenRotator(QObject *parent, const QVariantList &);
  void updateOrientation();
  void startProgress();
  void updateProgress();
  ~ScreenRotator();

  const static QString NAME;
  const static QString CONFIG;
  const static QString ENABLED;
  const static QString LOCKED;
  const static QString DELAY_DURATION;

  const static int DEFAULT_DELAY_DURATION;

  bool isEnabled();
  bool isLocked();
  int getDelayDuration();

  void refreshConfig();

private:
  QOrientationSensor *sensor;
  int progress;
  QTimer timer;
  QOrientationReading::Orientation currentOrientation;
  QProcess helper;

  KConfigGroup group;
};

#endif // MAINWINDOW_H
