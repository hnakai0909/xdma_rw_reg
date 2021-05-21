# xdma_rw_reg

DMA for PCI Express (PCIe) Subsystem + XDMA で、ホストPCからAXI4-Liteスレーブを叩くための最小構成

## ビルド方法
```sh
make
```

## 前提
### FPGA側
- DMA for PCI Express (PCIe) Subsystem に(オプションの)AXI4-Liteスレーブが生えていること
- (AXI Performance Monitorのフリーランニングカウンタ(`0x0000002c`)を読むことによるCounter diffの表示 : AXI Performance Monitorが接続されていること)

### ホストPC側
- XDMAドライバー(カーネルモジュール)がロード・動作していること
(cf. https://github.com/Xilinx/dma_ip_drivers/blob/master/XDMA/linux-kernel/readme.txt)

## 参考元
https://github.com/Xilinx/dma_ip_drivers/blob/master/XDMA/linux-kernel/tools/reg_rw.c
本プログラムは、おおむね、上のreg_rw.cからコマンドライン解釈部分を除いたものといえる。
