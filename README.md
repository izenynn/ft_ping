# ft_ping

## Info

This project is about recoding the ping command.

- Status: finished
- Result: 125%
- Observations: taken `inetutils-2.0` implementation as reference.

## Compile and use

```bash
make
```

```bash
./ft_ping [OPTION...] HOST ...
```

## Features

Run `./ft_ping -h` to see a full list of all options.

```bash
./ping -h
Usage: [OPTION...] HOST ...
Send ICMP ECHO_REQUEST packets to network hosts.

 Options controlling ICMP request types:

 Options valid for all request types:
  -c, --count=VALUE          stop after sending N packets
  -i, --interval=VALUE       wait N seconds between sending each packet
  -n, --numeric              do not resolve host addresses
      --ttl=VALUE            specify N as time-to-live
  -v, --verbose              verbose output
  -w, --timeout=VALUE        stop after N seconds
  -W, --linger=VALUE         number of seconds to wait for response

 Options valid for --echo requests:
  -f, --flood                flood ping
  -l, --preload=VALUE        send N packets as fast as possible before falling into normal made of behavior
  -p, --pattern=VALUE        fill ICMP packet with given pattern
  -s, --size=VALUE           send N data octets

  -h, --help                 give this help list
      --usage                give a short usage message
  -V, --version              print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to <me@izenynn.com>.
```

##
[![forthebadge](https://forthebadge.com/images/badges/made-with-c.svg)](https://forthebadge.com)
[![forthebadge](https://forthebadge.com/images/badges/for-you.svg)](https://forthebadge.com)
