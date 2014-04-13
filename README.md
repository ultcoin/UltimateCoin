# UltimateCoin [ULT]
http://www.ultcoin.org/

![UltimateCoin](http://i.imgur.com/68STVQC.jpg)

## What is UltimateCoin?
UltimateCoin is like Bitcoin, but based on Litecoin. It is aimed to be one of the major coins along side BTC, LTC & Doge!
http://www.ultcoin.org/

## IRC
irc.freenode.net Channel: #ultimatecoin

## License
UltimateCoin is released under the terms of the MIT license. See [COPYING](COPYING)
for more information or see http://opensource.org/licenses/MIT.

## Development and Contributions
Developers work in their own trees, then submit pull requests when they think
their feature or bug fix is ready.



### How to build UltimateCoin

    sudo apt-get install build-essential \
                         libssl-dev \
                         libdb5.1++-dev \
                         libboost-all-dev \
                         libqrencode-dev \
                         libminiupnpc-dev

    cd src/
    make -f makefile.unix USE_UPNP=1 USE_IPV6=1 USE_QRCODE=1

### Ports
RPC 21521
P2P 21520
