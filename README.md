# XIAO ROUND DISPLAY Driver


This repository contains an ESP-IDF driver for a Xiao Round display module. This module uses GC9A01 controller for LCD and CST816S controller for touch screen. Refer [Xiao Documentation](https://wiki.seeedstudio.com/seeedstudio_round_display_usage/) for more detailed.


## Using the component

Run the following command in your ESP-IDF project to install this component:
```bash
idf.py add-dependency "bienxanh1901/xiao_round_display"
```

## Example

To run the provided example, create it as follows:

```bash
idf.py create-project-from-example "bienxanh1901/xiao_round_display:xiao-round-display-example"
```

Then build as usual:
```bash
cd xiao-round-display-example
idf.py build
```

And flash it to the board:
```bash
idf.py -p PORT flash monitor
```

The example uses show the hardware test for LCD, touch and SD card.

## License

This component is provided under Apache 2.0 license, see [LICENSE](LICENSE.md) file for details.

## Contributing

Please check [CONTRIBUTING.md](CONTRIBUTING.md) for contribution guidelines.
