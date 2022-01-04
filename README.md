# kded_rotation

KDED module for handling automatic screen rotation on X11, with visual feedback before orientation change happens. Some assembly might be required.

# Installation

Run `./install_kded_rotation.sh` and install missing dependencies as needed. 

You'll most likely need `qt5-qtbase-devel`, `cmake-utils`, `extra-cmake-modules`, `iio-sensor-proxy`, `xrandr`, `qt5-qtsensors` and `kf5-kded-devel`. Depending on your distribution, these packages might have different names. CMake will tell you which packages it is missing.

# Usage

`orientation-helper` is where the actual screen rotation happens. This is achieved by calling `xrandr --rotation $value`, which works in most circumstances. You can adjust `orientation-helper` to fit your setup and reinstall to apply.

Configuration is read fresh prior to each rotation, it is located in ~/.config/kded_rotationrc file, ini format:
```
[General]
delay-duration=6
locked=true
enabled=true

```
allowing for granular control over rotation, with ```delay-totation``` configures timer to wait before performing rotation, bool flags ```locked``` and ```enabled``` allow for tandem of control over rotation (ex.: user locks rotation, enabled flag can be set/updated from a tablet-mode hardware sensor like 2-in-1 laptops).