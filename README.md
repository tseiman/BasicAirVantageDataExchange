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
### install app
```
instapp basicAirVantageDataExchangeComponent.wp76xx.update 192.168.2.2
```
or 
```
instapp basicAirVantageDataExchangeComponent.wp77xx.update 192.168.2.2
```
### run
**Note** the application is not auto starting. This ihas a clear reason as the default intervals are set pretty narrow so the device communicates a lot. Therefore the applicaiotn must be started manually:
```
ssh 192.168.2.2 -l root
app start basicAirVantageDataExchangeComponent
```
and to stop
```
app stop basicAirVantageDataExchangeComponent
```
you might want to follow the debug output (good way is to open a second console and ssh the WP device):
```
logread -f
```

