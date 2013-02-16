ok6410-uboot
============

ok6410 飞凌嵌入式开发板的uboot，主要功能增强：

1. 网卡DM9000A驱动
2. Linux下的dnw命令行工具
3. 支持SD卡和NAND flash启动
4. 烧写工具burnSD

使用方式
--------

1. 制作SD卡启动image

        make smdk6410_SDBOOT_config CROSS_COMPILE=... && make CROSS_COMPILE=... 
        ./tools/s3c6410_burnSD /dev/sdb u-boot.bin

其中/dev/sdb是SD卡的block device，u-boot.bin是编译好的uboot。

2. 制作NAND flash启动image

        make smdk6410_SDBOOT_config CROSS_COMPILE=... && make CROSS_COMPILE=... 

生成uboot.bin可以直接烧入开发板的nand flash，设置跳线就可以从新的uboot启动了。
