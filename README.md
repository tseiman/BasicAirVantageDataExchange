# BasicAirVantageDataExchange
A basic demonstration how to use AirVantage data management feature

## build
### setup leaf
follow the instructions from here to setup the leaf build environment tool from Legato/SierraWireless:
https://docs.legato.io/latest/confLeaf.html

### setup a leaf workspace
Setup the leaf workspace
**NOTE** that you may build for wp77xx - in that case substitute wp76xx by wp77xx)
**NOTE 2**: substitute swi-wp76_5.0.0 with the latest version given by leaf search

```
cd BasicAirVantageDataExchange
leaf search -t wp76xx -t latest
leaf setup SampleAV_WP76 -p swi-wp76_5.0.0
leaf shell
```
### build the app
```
make wp76xx
```
or in case of WP77
```
make wp77xx
```
