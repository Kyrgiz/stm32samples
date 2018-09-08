#include <stdint.h>
#include <wchar.h>
#include "usb_lib.h"
#ifdef EBUG
#include <string.h> // memcpy
#include "usart.h"
#endif

#define DEVICE_DESCRIPTOR_SIZE_BYTE         (18)
#define DEVICE_QALIFIER_SIZE_BYTE           (10)
#define STRING_LANG_DESCRIPTOR_SIZE_BYTE    (4)

const uint8_t USB_DeviceDescriptor[] = {
        DEVICE_DESCRIPTOR_SIZE_BYTE,   // bLength
        0x01,   // bDescriptorType - USB_DEVICE_DESC_TYPE
        0x00,   // bcdUSB_L
        0x02,   // bcdUSB_H
        0x00,   // bDeviceClass - USB_COMM
        0x00,   // bDeviceSubClass
        0x00,   // bDeviceProtocol
        0x40,   // bMaxPacketSize
        0x7b,   // idVendor_L PL2303: VID=0x067b, PID=0x2303
        0x06,   // idVendor_H
        0x03,   // idProduct_L
        0x23,   // idProduct_H
        0x00,   // bcdDevice_Ver_L
        0x01,   // bcdDevice_Ver_H
        0x01,   // iManufacturer
        0x02,   // iProduct
        0x00,   // iSerialNumber
        0x01    // bNumConfigurations
};

const uint8_t USB_DeviceQualifierDescriptor[] = {
        DEVICE_QALIFIER_SIZE_BYTE,   //bLength
        0x06,   // bDescriptorType
        0x00,   // bcdUSB_L
        0x02,   // bcdUSB_H
        0x00,   // bDeviceClass
        0x00,   // bDeviceSubClass
        0x00,   // bDeviceProtocol
        0x40,   // bMaxPacketSize0
        0x01,   // bNumConfigurations
        0x00    // Reserved
};

#if 0
const uint8_t USB_ConfigDescriptor[] = {
        /*Configuration Descriptor*/
        0x09, /* bLength: Configuration Descriptor size */
        0x02, /* bDescriptorType: Configuration */
        39,    /* wTotalLength:no of returned bytes */
        0x00,
        0x02, /* bNumInterfaces: 2 interface */
        0x01, /* bConfigurationValue: Configuration value */
        0x00, /* iConfiguration: Index of string descriptor describing the configuration */
        0x80, /* bmAttributes - Bus powered */
        0x32, /* MaxPower 100 mA */

        /*---------------------------------------------------------------------------*/

        /*Interface Descriptor */
        0x09, /* bLength: Interface Descriptor size */
        0x04, /* bDescriptorType: Interface */
        0x00, /* bInterfaceNumber: Number of Interface */
        0x00, /* bAlternateSetting: Alternate setting */
        0x01, /* bNumEndpoints: One endpoints used */
        0x02, /* bInterfaceClass: Communication Interface Class */
        0x02, /* bInterfaceSubClass: Abstract Control Model */
        0x01, /* bInterfaceProtocol: Common AT commands */
        0x00, /* iInterface: */

        /*Header Functional Descriptor*/
        0x05, /* bLength: Endpoint Descriptor size */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x00, /* bDescriptorSubtype: Header Func Desc */
        0x10, /* bcdCDC: spec release number */
        0x01,

        /*Call Management Functional Descriptor*/
        0x05, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x01, /* bDescriptorSubtype: Call Management Func Desc */
        0x00, /* bmCapabilities: D0+D1 */
        0x01, /* bDataInterface: 1 */

        /*ACM Functional Descriptor*/
        0x04, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x02, /* bDescriptorSubtype: Abstract Control Management desc */
        0x02, /* bmCapabilities */

        /*Union Functional Descriptor*/
        0x05, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x06, /* bDescriptorSubtype: Union func desc */
        0x00, /* bMasterInterface: Communication class interface */
        0x01, /* bSlaveInterface0: Data Class Interface */

        /*Endpoint 2 Descriptor*/
        0x07, /* bLength: Endpoint Descriptor size */
        0x05, /* bDescriptorType: Endpoint */
        0x81, /* bEndpointAddress IN1 */
        0x03, /* bmAttributes: Interrupt */
        0x08, /* wMaxPacketSize LO: */
        0x00, /* wMaxPacketSize HI: */
        0x10, /* bInterval: */
        /*---------------------------------------------------------------------------*/

        /*Data class interface descriptor*/
        0x09, /* bLength: Endpoint Descriptor size */
        0x04, /* bDescriptorType: */
        0x01, /* bInterfaceNumber: Number of Interface */
        0x00, /* bAlternateSetting: Alternate setting */
        0x02, /* bNumEndpoints: Two endpoints used */
        0x0A, /* bInterfaceClass: CDC */
        0x02, /* bInterfaceSubClass: */
        0x00, /* bInterfaceProtocol: */
        0x00, /* iInterface: */

        /*Endpoint IN2 Descriptor*/
        0x07, /* bLength: Endpoint Descriptor size */
        0x05, /* bDescriptorType: Endpoint */
        0x82, /* bEndpointAddress IN2 */
        0x02, /* bmAttributes: Bulk */
        64,   /* wMaxPacketSize: */
        0x00,
        0x00, /* bInterval: ignore for Bulk transfer */

        /*Endpoint OUT3 Descriptor*/
        0x07, /* bLength: Endpoint Descriptor size */
        0x05, /* bDescriptorType: Endpoint */
        0x03, /* bEndpointAddress */
        0x02, /* bmAttributes: Bulk */
        64,   /* wMaxPacketSize: */
        0,
        0x00 /* bInterval: ignore for Bulk transfer */
};
#endif

const uint8_t USB_ConfigDescriptor[] = {
        /*Configuration Descriptor*/
        0x09, /* bLength: Configuration Descriptor size */
        0x02, /* bDescriptorType: Configuration */
        53,   /* wTotalLength:no of returned bytes */
        0x00,
        0x01, /* bNumInterfaces: 1 interface */
        0x01, /* bConfigurationValue: Configuration value */
        0x00, /* iConfiguration: Index of string descriptor describing the configuration */
        0x80, /* bmAttributes - Bus powered */
        0x32, /* MaxPower 100 mA */

        /*---------------------------------------------------------------------------*/

        /*Interface Descriptor */
        0x09, /* bLength: Interface Descriptor size */
        0x04, /* bDescriptorType: Interface */
        0x00, /* bInterfaceNumber: Number of Interface */
        0x00, /* bAlternateSetting: Alternate setting */
        0x03, /* bNumEndpoints: 3 endpoints used */
        0x02, /* bInterfaceClass: Communication Interface Class */
        0x02, /* bInterfaceSubClass: Abstract Control Model */
        0x01, /* bInterfaceProtocol: Common AT commands */
        0x00, /* iInterface: */

        /*Header Functional Descriptor*/
        0x05, /* bLength: Endpoint Descriptor size */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x00, /* bDescriptorSubtype: Header Func Desc */
        0x10, /* bcdCDC: spec release number */
        0x01,

        /*Call Management Functional Descriptor*/
        0x05, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x01, /* bDescriptorSubtype: Call Management Func Desc */
        0x00, /* bmCapabilities: D0+D1 */
        0x01, /* bDataInterface: 1 */

        /*ACM Functional Descriptor*/
        0x04, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x02, /* bDescriptorSubtype: Abstract Control Management desc */
        0x02, /* bmCapabilities */

        /*Endpoint 1 Descriptor*/
        0x07, /* bLength: Endpoint Descriptor size */
        0x05, /* bDescriptorType: Endpoint */
        0x81, /* bEndpointAddress IN1 */
        0x03, /* bmAttributes: Interrupt */
        64, /* wMaxPacketSize LO: */
        0x00, /* wMaxPacketSize HI: */
        0x10, /* bInterval: */
        /*---------------------------------------------------------------------------*/

        /*Endpoint IN3 Descriptor*/
        0x07, /* bLength: Endpoint Descriptor size */
        0x05, /* bDescriptorType: Endpoint */
        0x83, /* bEndpointAddress IN3 */
        0x02, /* bmAttributes: Bulk */
        64,   /* wMaxPacketSize: */
        0x00,
        0x00, /* bInterval: ignore for Bulk transfer */

        /*Endpoint OUT2 Descriptor*/
        0x07, /* bLength: Endpoint Descriptor size */
        0x05, /* bDescriptorType: Endpoint */
        0x02, /* bEndpointAddress */
        0x02, /* bmAttributes: Bulk */
        64,   /* wMaxPacketSize: */
        0x00,
        0x00 /* bInterval: ignore for Bulk transfer */
};

const uint8_t USB_StringLangDescriptor[] = {
        STRING_LANG_DESCRIPTOR_SIZE_BYTE,   //bLength
        0x03,   //bDescriptorType
        0x09,   //wLANGID_L
        0x04    //wLANGID_H
};

//~ typedef struct{
    //~ uint8_t  bLength;
    //~ uint8_t  bDescriptorType;
    //~ uint16_t bString[];
//~ } StringDescriptor;

#define _USB_STRING_(name, str)                  \
const struct name \
{                          \
        uint8_t  bLength;                       \
        uint8_t  bDescriptorType;               \
        wchar_t  bString[(sizeof(str) - 2) / 2]; \
    \
} \
name = {sizeof(name), 0x03, str};

_USB_STRING_(USB_StringSerialDescriptor, L"0.01")
_USB_STRING_(USB_StringManufacturingDescriptor, L"Russia, SAO RAS")
_USB_STRING_(USB_StringProdDescriptor, L"TSYS01 sensors controller")

usb_dev_t USB_Dev;
ep_t endpoints[MAX_ENDPOINTS];

/*
bmRequestType: 76543210
7    direction: 0 - host->device, 1 - device->host
65   type: 0 - standard, 1 - class, 2 - vendor
4..0 getter: 0 - device, 1 - interface, 2 - endpoint, 3 - other
*/
/**
 * Endpoint0 (control) handler
 * @param ep - endpoint state
 * @return data written to EP0R
 */
uint16_t Enumerate_Handler(ep_t ep){
    config_pack_t *packet = (config_pack_t *)ep.rx_buf;
    uint16_t status = 0; // bus powered
    void wr0(const uint8_t *buf, uint16_t size){
        if(packet->wLength < size) size = packet->wLength;
        EP_WriteIRQ(0, buf, size);
    }
    if ((ep.rx_flag) && (ep.setup_flag)){
        if (packet -> bmRequestType == 0x80){ // get device properties
            switch(packet->bRequest){
                case GET_DESCRIPTOR:
                    switch(packet->wValue){
                        case DEVICE_DESCRIPTOR:
                            wr0(USB_DeviceDescriptor, sizeof(USB_DeviceDescriptor));
                        break;
                        case CONFIGURATION_DESCRIPTOR:
                            wr0(USB_ConfigDescriptor, sizeof(USB_ConfigDescriptor));
                        break;
                        case STRING_LANG_DESCRIPTOR:
                            wr0(USB_StringLangDescriptor, STRING_LANG_DESCRIPTOR_SIZE_BYTE);
                        break;
                        case STRING_MAN_DESCRIPTOR:
                            wr0((const uint8_t *)&USB_StringManufacturingDescriptor, USB_StringManufacturingDescriptor.bLength);
                        break;
                        case STRING_PROD_DESCRIPTOR:
                            wr0((const uint8_t *)&USB_StringProdDescriptor, USB_StringProdDescriptor.bLength);
                        break;
                        case STRING_SN_DESCRIPTOR:
                            wr0((const uint8_t *)&USB_StringSerialDescriptor, USB_StringSerialDescriptor.bLength);
                        break;
                        case DEVICE_QALIFIER_DESCRIPTOR:
                            wr0(USB_DeviceQualifierDescriptor, DEVICE_QALIFIER_SIZE_BYTE);
                        break;
                        default:
                            MSG("val");
                            printuhex(packet->wValue);
                            newline();
                    }
                break;
                case GET_STATUS:
                    EP_WriteIRQ(0, (uint8_t *)&status, 2); // send status: Bus Powered
                break;
                default:
                    MSG("req");
                    printuhex(packet->bRequest);
                    newline();
            }
            //��� ��� �� �� ������� ������ � ���������� ������ ���������, �� �������������
            ep.status = SET_NAK_RX(ep.status);
            ep.status = SET_VALID_TX(ep.status);
        }else if(packet->bmRequestType == 0x00){ // set device properties
            switch(packet->bRequest){
                case SET_ADDRESS:
                    //����� ��������� ����� � DADDR ������, ��� ��� ���� ������� �������������
                    //������ �� ������ �������
                    USB_Dev.USB_Addr = packet -> wValue;
                break;
                case SET_CONFIGURATION:
                    //������������� ��������� � "����������������"
                    USB_Dev.USB_Status = USB_CONFIGURE_STATE;
                break;
                default:
                    MSG("req");
                    printuhex(packet->bRequest);
                    newline();
            }
            //���������� ������������� ������
            EP_WriteIRQ(0, (uint8_t *)0, 0);
            //��� ��� �� �� ������� ������ � ���������� ������ ���������, �� �������������
            ep.status = SET_NAK_RX(ep.status);
            ep.status = SET_VALID_TX(ep.status);
        }else if(packet -> bmRequestType == 0x02){
            if (packet->bRequest == CLEAR_FEATURE){
                //���������� ������������� ������
                EP_WriteIRQ(0, (uint8_t *)0, 0);
                //��� ��� �� �� ������� ������ � ���������� ������ ���������, �� �������������
                ep.status = SET_NAK_RX(ep.status);
                ep.status = SET_VALID_TX(ep.status);
            }else{
                MSG("req");
                printuhex(packet->bRequest);
                newline();
            }
        }
    } else
    if (ep.rx_flag){    //������������ ������ ������
        //��� ��� ������������ �� ����� ��������� ����������
        //�� ���������� DTOG�
        ep.status = CLEAR_DTOG_RX(ep.status);
        ep.status = CLEAR_DTOG_TX(ep.status);
        //��� ��� �� ������� ����� ������ �� �����, �������������
        ep.status = SET_VALID_RX(ep.status);
        ep.status = SET_STALL_TX(ep.status);
    } else
    if (ep.tx_flag){    //�������� �������� ������
        //���� ���������� �� ����� ����� �� ��������� � ������� ����������
        if ((USB -> DADDR & USB_DADDR_ADD) != USB_Dev.USB_Addr){
            //����������� ����� ����� ����������
            USB -> DADDR = USB_DADDR_EF | USB_Dev.USB_Addr;
            //������������� ��������� � "�����������"
            USB_Dev.USB_Status = USB_ADRESSED_STATE;
        }
        //����� ����������, ������� DTOG
        ep.status = CLEAR_DTOG_RX(ep.status);
        ep.status = CLEAR_DTOG_TX(ep.status);
        //������� ����� ������, ��� ��������� ������ ������ (������ ��� ��������)
        //������� Rx � Tx � VALID
        ep.status = SET_VALID_RX(ep.status);
        ep.status = SET_VALID_TX(ep.status);
    }
    //�������� ep.status ����� �������� � EPnR � �������������� �������� ��� �����������
    //�������������� ����
    return ep.status;
}

/*
 * ������������� �������� �����
 * number - ����� (0...7)
 * type - ��� �������� ����� (EP_TYPE_BULK, EP_TYPE_CONTROL, EP_TYPE_ISO, EP_TYPE_INTERRUPT)
 * addr_tx - ����� ����������� ������ � ��������� USB
 * addr_rx - ����� ��������� ������ � ��������� USB
 * ������ ��������� ������ - ������������� 64 �����
 * uint16_t (*func)(ep_t *ep) - ����� ������� ����������� ������� ����������� ����� (����������)
 */
void EP_Init(uint8_t number, uint8_t type, uint16_t addr_tx, uint16_t addr_rx, uint16_t (*func)(ep_t ep)){
    USB -> EPnR[number] = (type << 9) | (number & USB_EPnR_EA);
    USB -> EPnR[number] ^= USB_EPnR_STAT_RX | USB_EPnR_STAT_TX_1;
    USB_BTABLE -> EP[number].USB_ADDR_TX = addr_tx;
    USB_BTABLE -> EP[number].USB_COUNT_TX = 0;
    USB_BTABLE -> EP[number].USB_ADDR_RX = addr_rx;
    USB_BTABLE -> EP[number].USB_COUNT_RX = 0x8400; // buffer size (64 bytes): Table127 of RM: BL_SIZE=1, NUM_BLOCK=1
    endpoints[number].func = func;
    endpoints[number].tx_buf = (uint16_t *)(USB_BTABLE_BASE + addr_tx);
    endpoints[number].rx_buf = (uint8_t *)(USB_BTABLE_BASE + addr_rx);
}

//���������� ���������� USB
void usb_isr(){
    uint8_t n;
    if (USB -> ISTR & USB_ISTR_RESET){
        // Reinit registers
        USB -> CNTR = USB_CNTR_RESETM | USB_CNTR_CTRM;
        USB -> ISTR = 0;
        // Endpoint 0 - CONTROL (128 bytes for TX and 64 for RX)
        EP_Init(0, EP_TYPE_CONTROL, 64, 192, Enumerate_Handler);
        //�������� ����� ����������
        USB -> DADDR = USB_DADDR_EF;
        //����������� ��������� � DEFAULT (�������� ����������)
        USB_Dev.USB_Status = USB_DEFAULT_STATE;
    }
    if (USB -> ISTR & USB_ISTR_CTR){
        //���������� ����� �������� �����, ��������� ����������
        n = USB -> ISTR & USB_ISTR_EPID;
        //�������� ���������� �������� ����
        endpoints[n].rx_cnt = USB_BTABLE -> EP[n].USB_COUNT_RX;
        //�������� ���������� EPnR ���� �������� �����
        endpoints[n].status = USB -> EPnR[n];
        //���������� ������
        endpoints[n].rx_flag = 0;
        endpoints[n].tx_flag = 0;
        endpoints[n].setup_flag = 0;
        //������������� ������ ������
        if (endpoints[n].status & USB_EPnR_CTR_RX) endpoints[n].rx_flag = 1;
        if (endpoints[n].status & USB_EPnR_SETUP) endpoints[n].setup_flag = 1;
        if (endpoints[n].status & USB_EPnR_CTR_TX) endpoints[n].tx_flag = 1;
        //�������� �������-���������� ������� �������� �����
        //� ����������� ������� � ������� EPnR ����������
        endpoints[n].status = endpoints[n].func(endpoints[n]);
        //�� ������ ��������� DTOG
        endpoints[n].status = KEEP_DTOG_TX(endpoints[n].status);
        endpoints[n].status = KEEP_DTOG_RX(endpoints[n].status);
        //������� ����� ������ � ��������
        endpoints[n].status = CLEAR_CTR_RX(endpoints[n].status);
        endpoints[n].status = CLEAR_CTR_TX(endpoints[n].status);
        USB -> EPnR[n] = endpoints[n].status;
    }
}
/*
 * ������� ������ ������� � ����� �������� ����� (����� �� ����������)
 * number - ����� �������� �����
 * *buf - ����� ������� � ������������� �������
 * size - ������ �������
 */
void EP_WriteIRQ(uint8_t number, const uint8_t *buf, uint16_t size){
    uint8_t i;
/*
 * �������� �������
 * ��-�� ������ ������ � ������� USB/CAN SRAM � 8-������ ��������
 * �������� ����������� ������ � 16-���, �������������� ������ ������
 * �� 2, ���� �� ��� ������, ��� ������ �� 2 + 1 ���� ��������
 */
    uint16_t temp = (size & 0x0001) ? (size + 1) / 2 : size / 2;
    uint16_t *buf16 = (uint16_t *)buf;
    for (i = 0; i < temp; i++){
        endpoints[number].tx_buf[i] = buf16[i];
    }
    //���������� ������������ ����
    USB_BTABLE -> EP[number].USB_COUNT_TX = size;
}
/*
 * ������� ������ ������� � ����� �������� ����� (����� ����� ����������)
 * number - ����� �������� �����
 * *buf - ����� ������� � ������������� �������
 * size - ������ �������
 */
void EP_Write(uint8_t number, const uint8_t *buf, uint16_t size){
    uint8_t i;
    uint16_t status = USB -> EPnR[number];
/*
 * �������� �������
 * ��-�� ������ ������ � ������� USB/CAN SRAM � 8-������ ��������
 * �������� ����������� ������ � 16-���, �������������� ������ ������
 * �� 2, ���� �� ��� ������, ��� ������ �� 2 + 1 ���� ��������
 */
    uint16_t temp = (size & 0x0001) ? (size + 1) / 2 : size / 2;
    uint16_t *buf16 = (uint16_t *)buf;
    for (i = 0; i < temp; i++){
        endpoints[number].tx_buf[i] = buf16[i];
    }
    //���������� ������������ ����
    USB_BTABLE -> EP[number].USB_COUNT_TX = size;

    status = SET_NAK_RX(status);        //RX � NAK
    status = SET_VALID_TX(status);      //TX � VALID
    status = KEEP_DTOG_TX(status);
    status = KEEP_DTOG_RX(status);
    USB -> EPnR[number] = status;
}

/*
 * ������� ������ ������� �� ������ �������� �����
 * number - ����� �������� �����
 * *buf - ����� ������� ���� ��������� ������
 */
void EP_Read(uint8_t number, uint8_t *buf){
    uint16_t i;
    for (i = 0; i < endpoints[number].rx_cnt; i++){
        buf[i] = endpoints[number].rx_buf[i];
    }
}
//������� ��������� ��������� ���������� USB
uint8_t USB_GetState(){
    return USB_Dev.USB_Status;
}
