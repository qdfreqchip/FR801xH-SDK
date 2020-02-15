
1、使用说明：
短按KEY1可以实现切换模式
PICTURE_UPDATE：短按KEY2可以刷新图片，
SENSOR_DATA：读取传感器数据模式，该模式下显示屏可以显示 温湿度、气压计（含温度）、加速度计（运动计数功能）的数据
SPEAKER_FROM_FLASH:通过手机端app FR8010_LOAD_SBC,搜索到“Simple Peripheral”，连接之后，发送音频数据，FR8016H将音频数据写入flash中，按下KEY2后，喇叭即可播放音频、

编译下载，运行就好了，
然后按K2，会切换图片。

K1：切换工作模式
K2:  切换图片，播放音频

2、flash分区：

音频存在0x60000~0x80000之间   


 3、需要短接的引脚

SCL     PC6 （使用jlink烧录时，不能短接该引脚）
SDA    PC7（使用jlink烧录时，不能短接该引脚）

PASD  PA1  （音频功放使能）
K1       PC5
K2       PD6
TXD     PA2
RXD     PA3

4. gcc toolchain is not ready for this project.