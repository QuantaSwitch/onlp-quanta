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
 * SFPI Interface for the Quanta LY2
 *
 ***********************************************************/
#include <powerpc_quanta_ly2_r0/powerpc_quanta_ly2_r0_config.h>
#include <onlp/platformi/sfpi.h>
#include <onlplib/sfp.h>
#include "powerpc_quanta_ly2_r0_log.h"

#include <unistd.h>
#include <fcntl.h>

/**
 * This table maps the presence gpio, reset gpio, and eeprom file
 * for each SFP port.
 */
typedef struct sfpmap_s {
    int port;
    const char*  mod_present_gpio;
    const char* reset_gpio;
    const char* eeprom;
} sfpmap_t;

static sfpmap_t sfpmap__[] =
    {
        {  1, "/sys/class/gpio/gpio168/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-14/14-0050/eeprom" },
        {  2, "/sys/class/gpio/gpio169/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-15/15-0050/eeprom" },
        {  3, "/sys/class/gpio/gpio170/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-16/16-0050/eeprom" },
        {  4, "/sys/class/gpio/gpio171/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-17/17-0050/eeprom" },
        {  5, "/sys/class/gpio/gpio172/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-18/18-0050/eeprom" },
        {  6, "/sys/class/gpio/gpio173/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-19/19-0050/eeprom" },
        {  7, "/sys/class/gpio/gpio174/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-20/20-0050/eeprom" },
        {  8, "/sys/class/gpio/gpio175/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-21/21-0050/eeprom" },
        {  9, "/sys/class/gpio/gpio176/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-22/22-0050/eeprom" },
        { 10, "/sys/class/gpio/gpio177/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-23/23-0050/eeprom" },
        { 11, "/sys/class/gpio/gpio178/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-24/24-0050/eeprom" },
        { 12, "/sys/class/gpio/gpio179/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-25/25-0050/eeprom" },
        { 13, "/sys/class/gpio/gpio180/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-26/26-0050/eeprom" },
        { 14, "/sys/class/gpio/gpio181/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-27/27-0050/eeprom" },
        { 15, "/sys/class/gpio/gpio182/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-28/28-0050/eeprom" },
        { 16, "/sys/class/gpio/gpio183/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-29/29-0050/eeprom" },
        { 17, "/sys/class/gpio/gpio144/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-30/30-0050/eeprom" },
        { 18, "/sys/class/gpio/gpio145/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-31/31-0050/eeprom" },
        { 19, "/sys/class/gpio/gpio146/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-32/32-0050/eeprom" },
        { 20, "/sys/class/gpio/gpio147/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-33/33-0050/eeprom" },
        { 21, "/sys/class/gpio/gpio148/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-34/34-0050/eeprom" },
        { 22, "/sys/class/gpio/gpio149/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-35/35-0050/eeprom" },
        { 23, "/sys/class/gpio/gpio150/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-36/36-0050/eeprom" },
        { 24, "/sys/class/gpio/gpio151/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-37/37-0050/eeprom" },
        { 25, "/sys/class/gpio/gpio152/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-38/38-0050/eeprom" },
        { 26, "/sys/class/gpio/gpio153/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-39/39-0050/eeprom" },
        { 27, "/sys/class/gpio/gpio154/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-40/40-0050/eeprom" },
        { 28, "/sys/class/gpio/gpio155/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-41/41-0050/eeprom" },
        { 29, "/sys/class/gpio/gpio156/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-42/42-0050/eeprom" },
        { 30, "/sys/class/gpio/gpio157/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-43/43-0050/eeprom" },
        { 31, "/sys/class/gpio/gpio158/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-44/44-0050/eeprom" },
        { 32, "/sys/class/gpio/gpio159/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-45/45-0050/eeprom" },
        { 33, "/sys/class/gpio/gpio120/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-46/46-0050/eeprom" },
        { 34, "/sys/class/gpio/gpio121/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-47/47-0050/eeprom" },
        { 35, "/sys/class/gpio/gpio122/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-48/48-0050/eeprom" },
        { 36, "/sys/class/gpio/gpio123/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-49/49-0050/eeprom" },
        { 37, "/sys/class/gpio/gpio124/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-50/50-0050/eeprom" },
        { 38, "/sys/class/gpio/gpio125/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-51/51-0050/eeprom" },
        { 39, "/sys/class/gpio/gpio126/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-52/52-0050/eeprom" },
        { 40, "/sys/class/gpio/gpio127/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-53/53-0050/eeprom" },
        { 41, "/sys/class/gpio/gpio128/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-54/54-0050/eeprom" },
        { 42, "/sys/class/gpio/gpio129/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-55/55-0050/eeprom" },
        { 43, "/sys/class/gpio/gpio130/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-56/56-0050/eeprom" },
        { 44, "/sys/class/gpio/gpio131/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-57/57-0050/eeprom" },
        { 45, "/sys/class/gpio/gpio132/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-58/58-0050/eeprom" },
        { 46, "/sys/class/gpio/gpio133/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-59/59-0050/eeprom" },
        { 47, "/sys/class/gpio/gpio134/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-60/60-0050/eeprom" },
        { 48, "/sys/class/gpio/gpio135/value", NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-61/61-0050/eeprom" },
        { 49,  NULL, NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-10/10-0050/eeprom" },
        { 50,  NULL, NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-11/11-0050/eeprom" },
        { 51,  NULL, NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-12/12-0050/eeprom" },
        { 52,  NULL, NULL, "/sys/devices/soc.0/ffe03100.i2c/i2c-1/i2c-13/13-0050/eeprom" },
  };


int
onlp_sfpi_init(void)
{
    return ONLP_STATUS_OK;
}

int
onlp_sfpi_bitmap_get(onlp_sfp_bitmap_t* bmap)
{
    int p;

    for(p = 0; p < 52; p++) {
        AIM_BITMAP_SET(bmap, p);
    }

    return ONLP_STATUS_OK;
}

#define SFP_GET(_port) (sfpmap__ + _port)

int
onlp_sfpi_is_present(int port)
{
    sfpmap_t* sfp = SFP_GET(port);
    if(sfp->mod_present_gpio) {
        return onlplib_sfp_is_present_file(sfp->mod_present_gpio,
                                           /* Present */ "1\n",
                                           /* Absent */  "0\n");
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
onlp_sfpi_reset(int port)
{
    sfpmap_t* sfp = SFP_GET(port);
    if(sfp->reset_gpio) {
        return onlplib_sfp_reset_file(sfp->reset_gpio,
                                      "0\n",
                                      POWERPC_QUANTA_LY2_R0_CONFIG_PHY_RESET_DELAY_MS,
                                      "1\n");
    }
    else {
        return ONLP_STATUS_E_UNSUPPORTED;
    }
}

