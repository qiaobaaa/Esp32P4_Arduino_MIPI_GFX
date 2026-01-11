#include "displays_config.h"
#define PCA9536_INPUT_REG    0x00        // 输入寄存器
#define PCA9536_OUTPUT_REG   0x01        // 输出寄存器
#define PCA9536_POLARITY_REG 0x02        // 极性反转寄存器
#define PCA9536_CONFIG_REG   0x03        // 配置寄存器

// PCA9536引脚定义
#define PCA9536_PIN0         (1 << 0)
#define PCA9536_PIN1         (1 << 1)
#define PCA9536_PIN2         (1 << 2)
#define PCA9536_PIN3         (1 << 3)

int set_display_backlight(DEV_I2C_Port port, uint8_t brightness)
{
    DEV_I2C_Set_Slave_Addr(&port.dev, display_cfg.i2c_address);
	DEV_I2C_Write_Byte(port.dev,PCA9536_OUTPUT_REG, brightness);
    return 0;
}

bool display_init(DEV_I2C_Port port)
{
    DEV_I2C_Set_Slave_Addr(&port.dev, display_cfg.i2c_address);
	DEV_I2C_Write_Byte(port.dev,PCA9536_CONFIG_REG, 0x00);
	uint16_t current;
    current = DEV_I2C_Read_Word(port.dev,PCA9536_OUTPUT_REG);
    printf("%d\n",current);
    
    DEV_I2C_Write_Byte(port.dev,PCA9536_OUTPUT_REG, current | PCA9536_PIN1);
	
	current = DEV_I2C_Read_Word(port.dev,PCA9536_OUTPUT_REG);
	printf("%d\n",current);
    DEV_I2C_Write_Byte(port.dev,PCA9536_OUTPUT_REG, current | PCA9536_PIN0);
    vTaskDelay(10);
	
	
	current = DEV_I2C_Read_Word(port.dev,PCA9536_OUTPUT_REG);
	DEV_I2C_Write_Byte(port.dev,PCA9536_OUTPUT_REG, current & ~PCA9536_PIN0);
	vTaskDelay(30);
	
	current = DEV_I2C_Read_Word(port.dev,PCA9536_OUTPUT_REG);
	printf("%d\n",current);
    DEV_I2C_Write_Byte(port.dev,PCA9536_OUTPUT_REG, current | PCA9536_PIN0);
    vTaskDelay(50);
	

    return true;
}
