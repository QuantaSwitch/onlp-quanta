/************************************************************
 * <bsn.cl fy=2014 v=onl>
 *
 *           Copyright 2014 Big Switch Networks, Inc.
 *
 * Licensed under the Eclipse Public License, Version 1.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *        http://www.eclipse.org/legal/epl-v10.html
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the
 * License.
 *
 * </bsn.cl>
 ************************************************************
 *
 * SFPI Interface for the Quanta LY8
 *
 ***********************************************************/
#include <x86_64_quanta_ly8_rangeley_r0/x86_64_quanta_ly8_rangeley_r0_config.h>
#include <x86_64_quanta_ly8_rangeley_r0/x86_64_quanta_ly8_rangeley_r0_gpio_table.h>
#include <onlp/platformi/sfpi.h>
#include <onlplib/sfp.h>
#include "x86_64_quanta_ly8_rangeley_r0_log.h"

#include <unistd.h>
#include <fcntl.h>

#include <quanta_lib/gpio.h>

/**
 * This table maps the presence gpio, reset gpio, and eeprom file
 * for each SFP port.
 */
typedef struct sfpmap_s {
    int port;
    int present_gpio;
    const char* reset_gpio;
    const char* eeprom;
    const char* dom;
} sfpmap_t;

static sfpmap_t sfpmap__[] =
    {
        {  1, PCA9698_1_GPIO_SFP_1_PRSNT_N /* 168 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-32/32-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-32/32-0051/eeprom" },
        {  2, PCA9698_1_GPIO_SFP_2_PRSNT_N /* 172 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-33/33-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-33/33-0051/eeprom" },
        {  3, PCA9698_1_GPIO_SFP_3_PRSNT_N /* 176 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-34/34-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-34/34-0051/eeprom" },
        {  4, PCA9698_1_GPIO_SFP_4_PRSNT_N /* 180 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-35/35-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-35/35-0051/eeprom" },
        {  5, PCA9698_1_GPIO_SFP_5_PRSNT_N /* 184 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-36/36-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-36/36-0051/eeprom" },
        {  6, PCA9698_1_GPIO_SFP_6_PRSNT_N /* 188 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-37/37-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-37/37-0051/eeprom" },
        {  7, PCA9698_1_GPIO_SFP_7_PRSNT_N /* 192 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-38/38-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-38/38-0051/eeprom" },
        {  8, PCA9698_1_GPIO_SFP_8_PRSNT_N /* 196 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-39/39-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-39/39-0051/eeprom" },
        {  9, PCA9698_2_GPIO_SFP_9_PRSNT_N /* 208 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-40/40-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-40/40-0051/eeprom" },
        { 10, PCA9698_2_GPIO_SFP_10_PRSNT_N /* 212 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-41/41-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-41/41-0051/eeprom" },
        { 11, PCA9698_2_GPIO_SFP_11_PRSNT_N /* 216 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-42/42-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-42/42-0051/eeprom" },
        { 12, PCA9698_2_GPIO_SFP_12_PRSNT_N /* 220 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-43/43-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-43/43-0051/eeprom" },
        { 13, PCA9698_2_GPIO_SFP_13_PRSNT_N /* 224 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-44/44-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-44/44-0051/eeprom" },
        { 14, PCA9698_2_GPIO_SFP_14_PRSNT_N /* 228 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-45/45-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-45/45-0051/eeprom" },
        { 15, PCA9698_2_GPIO_SFP_15_PRSNT_N /* 232 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-46/46-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-46/46-0051/eeprom" },
        { 16, PCA9698_2_GPIO_SFP_16_PRSNT_N /* 236 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-47/47-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-47/47-0051/eeprom" },
        { 17, PCA9698_3_GPIO_SFP_17_PRSNT_N /* 248 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-48/48-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-48/48-0051/eeprom" },
        { 18, PCA9698_3_GPIO_SFP_18_PRSNT_N /* 252 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-49/49-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-49/49-0051/eeprom" },
        { 19, PCA9698_3_GPIO_SFP_19_PRSNT_N /* 256 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-50/50-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-50/50-0051/eeprom" },
        { 20, PCA9698_3_GPIO_SFP_20_PRSNT_N /* 260 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-51/51-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-51/51-0051/eeprom" },
        { 21, PCA9698_3_GPIO_SFP_21_PRSNT_N /* 264 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-52/52-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-52/52-0051/eeprom" },
        { 22, PCA9698_3_GPIO_SFP_22_PRSNT_N /* 268 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-53/53-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-53/53-0051/eeprom" },
        { 23, PCA9698_3_GPIO_SFP_23_PRSNT_N /* 272 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-54/54-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-54/54-0051/eeprom" },
        { 24, PCA9698_3_GPIO_SFP_24_PRSNT_N /* 276 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-55/55-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-17/i2c-55/55-0051/eeprom" },
        { 25, PCA9698_4_GPIO_SFP_25_PRSNT_N /* 288 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-56/56-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-56/56-0051/eeprom" },
        { 26, PCA9698_4_GPIO_SFP_26_PRSNT_N /* 292 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-57/57-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-57/57-0051/eeprom" },
        { 27, PCA9698_4_GPIO_SFP_27_PRSNT_N /* 296 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-58/58-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-58/58-0051/eeprom" },
        { 28, PCA9698_4_GPIO_SFP_28_PRSNT_N /* 300 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-59/59-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-59/59-0051/eeprom" },
        { 29, PCA9698_4_GPIO_SFP_29_PRSNT_N /* 304 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-60/60-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-60/60-0051/eeprom" },
        { 30, PCA9698_4_GPIO_SFP_30_PRSNT_N /* 308 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-61/61-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-61/61-0051/eeprom" },
        { 31, PCA9698_4_GPIO_SFP_31_PRSNT_N /* 312 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-62/62-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-62/62-0051/eeprom" },
        { 32, PCA9698_4_GPIO_SFP_32_PRSNT_N /* 316 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-63/63-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-63/63-0051/eeprom" },
        { 33, PCA9698_5_GPIO_SFP_33_PRSNT_N /* 328 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-64/64-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-64/64-0051/eeprom" },
        { 34, PCA9698_5_GPIO_SFP_34_PRSNT_N /* 332 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-65/65-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-65/65-0051/eeprom" },
        { 35, PCA9698_5_GPIO_SFP_35_PRSNT_N /* 336 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-66/66-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-66/66-0051/eeprom" },
        { 36, PCA9698_5_GPIO_SFP_36_PRSNT_N /* 340 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-67/67-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-67/67-0051/eeprom" },
        { 37, PCA9698_5_GPIO_SFP_37_PRSNT_N /* 344 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-68/68-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-68/68-0051/eeprom" },
        { 38, PCA9698_5_GPIO_SFP_38_PRSNT_N /* 348 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-69/69-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-69/69-0051/eeprom" },
        { 39, PCA9698_5_GPIO_SFP_39_PRSNT_N /* 352 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-70/70-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-70/70-0051/eeprom" },
        { 40, PCA9698_5_GPIO_SFP_40_PRSNT_N /* 356 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-71/71-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-71/71-0051/eeprom" },
        { 41, PCA9698_6_GPIO_SFP_41_PRSNT_N /* 368 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-72/72-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-72/72-0051/eeprom" },
        { 42, PCA9698_6_GPIO_SFP_42_PRSNT_N /* 372 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-73/73-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-73/73-0051/eeprom" },
        { 43, PCA9698_6_GPIO_SFP_43_PRSNT_N /* 376 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-74/74-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-74/74-0051/eeprom" },
        { 44, PCA9698_6_GPIO_SFP_44_PRSNT_N /* 380 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-75/75-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-75/75-0051/eeprom" },
        { 45, PCA9698_6_GPIO_SFP_45_PRSNT_N /* 384 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-76/76-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-76/76-0051/eeprom" },
        { 46, PCA9698_6_GPIO_SFP_46_PRSNT_N /* 388 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-77/77-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-77/77-0051/eeprom" },
        { 47, PCA9698_6_GPIO_SFP_47_PRSNT_N /* 392 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-78/78-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-78/78-0051/eeprom" },
        { 48, PCA9698_6_GPIO_SFP_48_PRSNT_N /* 396 */, NULL, "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-79/79-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-18/i2c-79/79-0051/eeprom" },
        { 49, QSFP_GPIO_PRSNT_49_N /* 426 */, "/sys/class/gpio/gpio424/value", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-21/i2c-80/80-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-21/i2c-80/80-0051/eeprom" },
        { 50, QSFP_GPIO_PRSNT_50_N /* 430 */, "/sys/class/gpio/gpio428/value", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-21/i2c-81/81-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-21/i2c-81/81-0051/eeprom" },
        { 51, QSFP_GPIO_PRSNT_51_N /* 434 */, "/sys/class/gpio/gpio432/value", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-21/i2c-82/82-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-21/i2c-82/82-0051/eeprom" },
        { 52, QSFP_GPIO_PRSNT_52_N /* 438 */, "/sys/class/gpio/gpio436/value", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-21/i2c-83/83-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-21/i2c-83/83-0051/eeprom" },
        { 69, QSFP_QDB_GPIO_PRSNT_69_N /* 442 */, "/sys/class/gpio/gpio432/value", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-23/i2c-88/88-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-23/i2c-88/88-0051/eeprom" },
        { 70, QSFP_QDB_GPIO_PRSNT_70_N /* 446 */, "/sys/class/gpio/gpio436/value", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-23/i2c-89/89-0050/eeprom", "/sys/devices/pci0000:00/0000:00:1f.3/i2c-0/i2c-23/i2c-89/89-0051/eeprom" },
  };

int
onlp_sfpi_init(void)
{
    int value = -1, ret;
    ret = pca953x_gpio_value_get(QSFP_EN_GPIO_P3V3_PW_EN, &value);
    if(ret == ONLP_STATUS_OK && value != GPIO_HIGH) {
        ret = pca953x_gpio_value_set(QSFP_EN_GPIO_P3V3_PW_EN, GPIO_HIGH);
    }

    if(ret == ONLP_STATUS_OK) {
        ret = pca953x_gpio_value_get(QSFP_QDB_GPIO_MOD_EN_N, &value);
        if(ret == ONLP_STATUS_OK && value != GPIO_LOW) {
            ret = pca953x_gpio_value_set(QSFP_QDB_GPIO_MOD_EN_N, GPIO_LOW);
        }
    }

    return ret;
}

int
onlp_sfpi_bitmap_get(onlp_sfp_bitmap_t* bmap)
{
    int p;

    for(p = 0; p < 54; p++) {
        AIM_BITMAP_SET(bmap, p);
    }

    return ONLP_STATUS_OK;
}

#define SFP_GET(_port) (sfpmap__ + _port)

int
onlp_sfpi_is_present(int port)
{
	int value = 0;
    sfpmap_t* sfp = SFP_GET(port);
    if(sfp->present_gpio > 0) {
		if(pca953x_gpio_value_get(sfp->present_gpio, &value) == ONLP_STATUS_OK)
			return (value == GPIO_LOW);
		else
			return ONLP_STATUS_E_MISSING;
    }
    else {
        /**
         * If we can open and read a byte from the EEPROM file
         * then we consider it present.
         */
        int fd = open(sfp->eeprom, O_RDONLY);
        if (fd < 0) {
            /* Not Present */
            return 0;
        }
        int rv;
        uint8_t byte;

        if(read(fd, &byte, 1) == 1) {
            /* Present */
            rv = 1;
        }
        else {
            /* No Present */
            rv = 0;
        }
        close(fd);
        return rv;
    }
}

int
onlp_sfpi_eeprom_read(int port, uint8_t data[256])
{
    sfpmap_t* sfp = SFP_GET(port);
    return onlplib_sfp_eeprom_read_file(sfp->eeprom, data);
}

int
onlp_sfpi_dom_read(int port, uint8_t data[256])
{
    sfpmap_t* sfp = SFP_GET(port);
    return onlplib_sfp_eeprom_read_file(sfp->dom, data);
}

