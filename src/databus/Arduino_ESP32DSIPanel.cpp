#include "Arduino_ESP32DSIPanel.h"
#define TAG "Arduino_ESP32DSIPanel"
#define LCD_CMD_SWRESET          0x01 

#if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32P4)
Arduino_ESP32DSIPanel::Arduino_ESP32DSIPanel(
    uint32_t hsync_pulse_width, uint32_t hsync_back_porch, uint32_t hsync_front_porch,
    uint32_t vsync_pulse_width, uint32_t vsync_back_porch, uint32_t vsync_front_porch,
    uint32_t prefer_speed,uint32_t lane_bit_rate /*新增成员变量*/)
    : _hsync_pulse_width(hsync_pulse_width), _hsync_back_porch(hsync_back_porch), _hsync_front_porch(hsync_front_porch),
      _vsync_pulse_width(vsync_pulse_width), _vsync_back_porch(vsync_back_porch), _vsync_front_porch(vsync_front_porch),
      _prefer_speed(prefer_speed),
	  _lane_bit_rate(lane_bit_rate)/*新增成员变量*/
{
}

bool Arduino_ESP32DSIPanel::begin(int16_t w, int16_t h, int32_t speed, const lcd_init_cmd_t *init_operations, size_t init_operations_len)
{
  if (speed == GFX_NOT_DEFINED)
  {
    if (_prefer_speed != GFX_NOT_DEFINED)
    {
      speed = _prefer_speed;
    }
    else
    {
      speed = 56000000L;
    }
  }

  esp_ldo_channel_handle_t ldo_mipi_phy = NULL;
  esp_ldo_channel_config_t ldo_mipi_phy_config = {
      .chan_id = EXAMPLE_MIPI_DSI_PHY_PWR_LDO_CHAN,
      .voltage_mv = EXAMPLE_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV,
  };
  ESP_ERROR_CHECK(esp_ldo_acquire_channel(&ldo_mipi_phy_config, &ldo_mipi_phy));
  ESP_LOGI(TAG, "MIPI DSI PHY Powered on");

  esp_lcd_dsi_bus_config_t bus_config = {
      .bus_id = 0,
      .num_data_lanes = 1,
      .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
      //.lane_bit_rate_mbps = _lane_bit_rate, /*新增成员变量*/
	  .lane_bit_rate_mbps = 700, /*新增成员变量*/
  };
  esp_lcd_dsi_bus_handle_t mipi_dsi_bus = NULL;

  ESP_ERROR_CHECK(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus));

  ESP_LOGI(TAG, "Install MIPI DSI LCD control panel");
  esp_lcd_dbi_io_config_t dbi_config = {
      .virtual_channel = 0,
      .lcd_cmd_bits = 8,
      .lcd_param_bits = 8,
  };
  esp_lcd_panel_io_handle_t io_handle = NULL;
  ESP_ERROR_CHECK(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &io_handle));

  esp_lcd_dpi_panel_config_t dpi_config = {
      .virtual_channel = 0,
      .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
      .dpi_clock_freq_mhz = speed / 1000000,
      .pixel_format = LCD_COLOR_PIXEL_FORMAT_RGB565,
      .num_fbs = 1,
      .video_timing = {
          .h_size = w,
          .v_size = h,
          .hsync_pulse_width = _hsync_pulse_width,
          .hsync_back_porch = _hsync_back_porch,
          .hsync_front_porch = _hsync_front_porch,
          .vsync_pulse_width = _vsync_pulse_width,
          .vsync_back_porch = _vsync_back_porch,
          .vsync_front_porch = _vsync_front_porch,
      },
      .flags = {
          .use_dma2d = true,
      },
  };

  const esp_lcd_panel_dev_config_t panel_config = {
      .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
      .bits_per_pixel = 16,
  };

  // Create MIPI DPI panel
  ESP_ERROR_CHECK(esp_lcd_new_panel_dpi(mipi_dsi_bus, &dpi_config, &_panel_handle));
  /*
  if (_panel_handle)
    { // Perform software reset
        ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io_handle, LCD_CMD_SWRESET, NULL, 0), TAG, "send command failed");
        vTaskDelay(pdMS_TO_TICKS(20));
    }
  
  
  ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io_handle, LCD_CMD_SWRESET, NULL, 0), TAG, "send command failed");
        vTaskDelay(pdMS_TO_TICKS(20));
  
  ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io_handle, LCD_CMD_SWRESET, NULL, 0), TAG, "send command failed");
    vTaskDelay(pdMS_TO_TICKS(150));
  esp_err_t sleep_out_ret = esp_lcd_panel_io_tx_param(io_handle, 0x11, NULL, 0);
	if (sleep_out_ret == ESP_OK)
	{
		ESP_LOGI(TAG, "Sent Sleep Out (0x11) command successfully.");
		printf("%d\n",666);
		vTaskDelay(pdMS_TO_TICKS(120)); // 等待屏幕退出休眠
	}
	else
	{
		ESP_LOGE(TAG, "Failed to send Sleep Out (0x11) command: %s", esp_err_to_name(sleep_out_ret));
		printf("%d\n",777);
	}
	 // 准备接收缓冲区
    uint8_t id_data[4] = {0}; // 准备4个字节的缓冲区，以防万一

    // 执行读命令 0x04 (read_display_id)
    // MIPI DSI读操作通常是异步的，ESP-IDF的实现会阻塞等待结果
    esp_err_t ret = esp_lcd_panel_io_rx_param(io_handle, 0xA1, id_data, 3); // 期望读取3个字节

    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Successfully read display ID!");
        ESP_LOGI(TAG, "ID[0]: 0x%02X", id_data[0]);
        ESP_LOGI(TAG, "ID[1]: 0x%02X", id_data[1]);
        ESP_LOGI(TAG, "ID[2]: 0x%02X", id_data[2]);
		printf("%d\n",888);
		printf("%d\n",id_data[0]);
		printf("%d\n",id_data[1]);
		printf("%d\n",id_data[2]);
        // 根据ST7701数据手册，ID通常是 0x77, 0x01, 加上其他版本号
        // 如果读到的不是全0或全FF，那就说明通信链路是通的！
    }
    else
    {
		printf("%d\n",999);
        ESP_LOGE(TAG, "Failed to read display ID. Error: %s", esp_err_to_name(ret));
        ESP_LOGE(TAG, "This likely indicates a hardware connection issue (power, reset, or MIPI DSI lines P/N reversed) or a fundamental configuration error.");
    }

    uint8_t power_mode_data[2] = {0}; // 准备2个字节以防万一

    // 执行读命令 0x0A (get_power_mode)
    ret = esp_lcd_panel_io_rx_param(io_handle, 0x0A, power_mode_data, 1); // 期望读取1个字节

    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Successfully read Power Mode!");
        ESP_LOGI(TAG, "Power Mode: 0x%02X", power_mode_data[0]);
        // 在Sleep Out之后，这个值通常不是0。如果读到非FF的值，比如0x9C, 0x8C等，就说明通信成功了。
		printf("%d\n",123456);
		printf("%d\n",power_mode_data[0]);
       // return ret;
    }
    else
    {
		printf("%d\n",21525575);
        ESP_LOGE(TAG, "Failed to read Power Mode. Error: %s", esp_err_to_name(ret));
        //return ESP_FAIL;
    }
	
	//esp_lcd_panel_reset(_panel_handle);
  //ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io_handle, LCD_CMD_SWRESET, NULL, 0), TAG, "send command failed");
  //      vTaskDelay(pdMS_TO_TICKS(20));
		*/
		
	
  for (int i = 0; i < init_operations_len; i++)
  {
    // Send command
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, init_operations[i].cmd, init_operations[i].data, init_operations[i].data_bytes));
    vTaskDelay(pdMS_TO_TICKS(init_operations[i].delay_ms));
  }
  ESP_LOGD(TAG, "send init commands success");

  ESP_ERROR_CHECK(esp_lcd_panel_init(_panel_handle));
  
  return true;
}

uint16_t *Arduino_ESP32DSIPanel::getFrameBuffer()
{
  void *frame_buffer = nullptr;
  ESP_ERROR_CHECK(esp_lcd_dpi_panel_get_frame_buffer(_panel_handle, 1, &frame_buffer));

  return ((uint16_t *)frame_buffer);
}

#endif // #if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32P4)
