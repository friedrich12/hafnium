setenv loadaddr 0x42000000
setenv imgname hafnium.bin

load ${loadaddr} ${devtype} ${imgname}
go ${loadaddr}

