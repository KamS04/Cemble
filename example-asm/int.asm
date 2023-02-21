data16 commList = { $0000 }
data16 devList = { $0000 }
data16 devIntList = { $0000 }
int2:
mul $2, r1
add [!commList], acu
rti
int3:
mov &[!devList], acu
rti
int4:
mov &[!devIntList], acu
rti