#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <zephyr.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include <device.h>
#include <gpio.h>


/**************************************************************************
 * macros
 **************************************************************************/

// Base UUID
//  0c49xxxx-c525-419f-af43-b7274ce3232a

// service
//      73e3
#define M_SVC_UUID \
    0x2a, 0x23, 0xe3, 0x4c, 0x27, 0xb7, 0x43, 0xaf, 0x9f, 0x41, 0x25, 0xc5, 0xe3, 0x73, 0x49, 0x0c


// characteristic
//      73e4
#define M_CHAR_UUID \
    0x2a, 0x23, 0xe3, 0x4c, 0x27, 0xb7, 0x43, 0xaf, 0x9f, 0x41, 0x25, 0xc5, 0xe4, 0x73, 0x49, 0x0c


/**************************************************************************
 * prototypes
 **************************************************************************/

static ssize_t write_ct(
        struct bt_conn *conn, const struct bt_gatt_attr *attr,
        const void *buf, u16_t len, u16_t offset, u8_t flags);


/**************************************************************************
 * const variables
 **************************************************************************/

static const struct bt_uuid_128 kUuidChar = BT_UUID_INIT_128(M_CHAR_UUID);


/**************************************************************************
 * private variables
 **************************************************************************/

static u8_t ct[1];
static struct device *dev_led;
static struct bt_uuid_128 kUuidSvc = BT_UUID_INIT_128(M_SVC_UUID);


/**************************************************************************
 * public variables
 **************************************************************************/

BT_GATT_SERVICE_DEFINE(led_svc,
    BT_GATT_PRIMARY_SERVICE(&kUuidSvc.uuid),
    BT_GATT_CHARACTERISTIC(&kUuidChar.uuid, BT_GATT_CHRC_WRITE,
                   BT_GATT_PERM_WRITE,
                   NULL, write_ct, ct),
);


/**************************************************************************
 * public functions
 **************************************************************************/

void led_init(void)
{
    dev_led = device_get_binding(DT_ALIAS_LED0_GPIOS_CONTROLLER);
    gpio_pin_configure(dev_led, DT_ALIAS_LED0_GPIOS_PIN, GPIO_DIR_OUT);
    gpio_pin_write(dev_led, DT_ALIAS_LED0_GPIOS_PIN, 0);
}


/**************************************************************************
 * private functions
 **************************************************************************/

static ssize_t write_ct(
        struct bt_conn *conn, const struct bt_gatt_attr *attr,
        const void *buf, u16_t len, u16_t offset, u8_t flags)
{
    u8_t *value = attr->user_data;

    if (offset + len > sizeof(ct)) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    memcpy(value + offset, buf, len);

    gpio_pin_write(dev_led, DT_ALIAS_LED0_GPIOS_PIN, value[0] ? 1 : 0);

    return len;
}
