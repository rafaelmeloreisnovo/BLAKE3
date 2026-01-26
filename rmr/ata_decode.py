#!/usr/bin/env python3
import struct

G="0123456789ABCDEFGHIJ"
def v20(n:int)->str:
    if n==0: return "0"
    s=[]
    while n>0:
        s.append(G[n%20]); n//=20
    return "".join(reversed(s))

PATH="ATA_OMEGA.bin"
with open(PATH,"rb") as f:
    magic=f.read(4)
    if magic!=b"RFA\0":
        raise SystemExit(f"magic errado: {magic!r}")
    sig,=struct.unpack("<Q", f.read(8))
    data=f.read()

recs=len(data)//24
print(f"[OK] magic=RFA\\0  hw_sig=0x{sig:016x}  hw_sig_v20={v20(sig)}  recs={recs}")
for i in range(min(recs, 60)):
    k,cyc,par=struct.unpack_from("<QQQ", data, i*24)
    print(f"[{i+1:02d}] k={k:02d}  cyc=0x{cyc:016x}  cyc_v20={v20(cyc)}  p={par}")
