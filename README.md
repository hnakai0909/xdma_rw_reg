# xdma_rw_reg

DMA for PCI Express (PCIe) Subsystem + XDMA で、ホストPCからAXI4-Liteスレーブを叩く(値を読み書きする)ための最小構成

## 前提
### FPGA側
- DMA for PCI Express (PCIe) Subsystem に(オプションの)AXI4-Liteスレーブが生えていること
- (AXI Performance Monitorのフリーランニングカウンタ(`0x0000002c`)を読むことによるCounter diffの表示 : AXI Performance Monitorが接続されていること)

### ホストPC側
- XDMAドライバー(カーネルモジュール)がロード・動作していること
(cf. https://github.com/Xilinx/dma_ip_drivers/blob/master/XDMA/linux-kernel/readme.txt)

## ビルド方法
```sh
make
```

## 実行方法
引数はなし
```sh
./reg_rw
```

## 関数
### `read_from_device()`
FPGAデバイスからホストPCへ32ビット値を読む。
- 引数：読み出し元アドレス。FPGA側(DMA for PCI Express (PCIe) SubsystemのIP)のAXI4-Liteバスにおけるアドレス。
- 返り値：読み出した値

### `write_to_device()`
ホストPCからFPGAデバイスへ32ビット値を書く。
- 引数：書き込み先アドレス。FPGA側(DMA for PCI Express (PCIe) SubsystemのIP)のAXI4-Liteバスにおけるアドレス。
- 返り値：なし

### `main()`
基本的な流れは、以下の通り。
1. `open()`
1. `mmap()`
1. 任意アドレス読み書き
1. `munmap()`
1. `close()`

## 編集するべき箇所
- `//#define DEBUG 1` : コメントを解除すればデバッグ出力モードになり出力が詳細になる。
- `#define MAP_SIZE` : FPGA側(DMA for PCI Express (PCIe) SubsystemのIP)でマッピングしているメモリ領域の大きさに応じて変更する。バイト単位。
- `char *device` : XDMAのキャラクタデバイス名。通常はxdma0_user。複数枚ボードがある場合は数字を変える。なお、現在マウントされているキャラクタデバイスは`ls /dev/xdma*`で確認できる。
- mmap以降、munmapまで：希望するアドレスの`read_from_device()`や`write_to_device()`を呼び出す。

これ以外は、基本的には触らなくていいはず。


## 参考元
https://github.com/Xilinx/dma_ip_drivers/blob/master/XDMA/linux-kernel/tools/reg_rw.c
本プログラムは、おおむね、上のreg_rw.cからコマンドライン解釈部分を除いたものといえる。
