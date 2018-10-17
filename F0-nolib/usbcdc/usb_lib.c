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

static usb_LineCoding lineCoding = {115200, 0, 0, 8};

const uint8_t USB_DeviceDescriptor[] = {
        DEVICE_DESCRIPTOR_SIZE_BYTE,   // bLength
        0x01,   // bDescriptorType - USB_DEVICE_DESC_TYPE
        0x10,   // bcdUSB_L - 1.10
        0x01,   // bcdUSB_H
        0x00,   // bDeviceClass - USB_COMM
        0x00,   // bDeviceSubClass
        0x00,   // bDeviceProtocol
        0x40,   // bMaxPacketSize
        0x7b,   // idVendor_L PL2303: VID=0x067b, PID=0x2303
        0x06,   // idVendor_H
        0x03,   // idProduct_L
        0x23,   // idProduct_H
        0x00,   // bcdDevice_Ver_L
        0x03,   // bcdDevice_Ver_H
        0x01,   // iManufacturer
        0x02,   // iProduct
        0x00,   // iSerialNumber
        0x01    // bNumConfigurations
};

const uint8_t USB_DeviceQualifierDescriptor[] = {
        DEVICE_QALIFIER_SIZE_BYTE,   //bLength
        0x06,   // bDescriptorType
        0x10,   // bcdUSB_L
        0x01,   // bcdUSB_H
        0x00,   // bDeviceClass
        0x00,   // bDeviceSubClass
        0x00,   // bDeviceProtocol
        0x40,   // bMaxPacketSize0
        0x01,   // bNumConfigurations
        0x00    // Reserved
};

const uint8_t USB_ConfigDescriptor[] = {
        /*Configuration Descriptor*/
        0x09, /* bLength: Configuration Descriptor size */
        0x02, /* bDescriptorType: Configuration */
        39,   /* wTotalLength:no of returned bytes */
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
        //0x02, /* bInterfaceClass: Communication Interface Class */
        //0x02, /* bInterfaceSubClass: Abstract Control Model */
        //0x01, /* bInterfaceProtocol: Common AT commands */
    0xff,
    0x00,
    0x00,
        0x00, /* iInterface: */
///////////////////////////////////////////////////
        /*Endpoint 1 Descriptor*/
        0x07, /* bLength: Endpoint Descriptor size */
        0x05, /* bDescriptorType: Endpoint */
        0x81, /* bEndpointAddress IN1 */
        0x03, /* bmAttributes: Interrupt */
        //0x40, /* wMaxPacketSize LO: */
        //0x00, /* wMaxPacketSize HI: */
    0x0a,
    0x00,
        //0x10, /* bInterval: */
    0x01,
        /*---------------------------------------------------------------------------*/

        /*Endpoint OUT2 Descriptor*/
        0x07, /* bLength: Endpoint Descriptor size */
        0x05, /* bDescriptorType: Endpoint */
        0x02, /* bEndpointAddress: OUT2 */
        0x02, /* bmAttributes: Bulk */
        0x40, /* wMaxPacketSize: */
        0x00,
        0x00, /* bInterval: ignore for Bulk transfer */

        /*Endpoint IN3 Descriptor*/
        0x07, /* bLength: Endpoint Descriptor size */
        0x05, /* bDescriptorType: Endpoint */
        //0x82, /* bEndpointAddress IN2 */
    0x83, // IN3
        0x02, /* bmAttributes: Bulk */
        0x40, /* wMaxPacketSize: 64 */
        0x00,
        0x00, /* bInterval: ignore for Bulk transfer */
};

#if 0
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
#endif

const uint8_t USB_StringLangDescriptor[] = {
        STRING_LANG_DESCRIPTOR_SIZE_BYTE,   //bLength
        0x03,   //bDescriptorType
        0x09,   //wLANGID_L
        0x04    //wLANGID_H
};

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

static void EP_Readx(uint8_t number, uint8_t *buf){
    uint32_t timeout = 100000;
    uint16_t status, i;
    status = USB -> EPnR[number];
    status = SET_VALID_RX(status);
    status = SET_NAK_TX(status);
    status = KEEP_DTOG_TX(status);
    status = KEEP_DTOG_RX(status);
    USB -> EPnR[number] = status;
    endpoints[number].rx_flag = 0;
    while (!endpoints[number].rx_flag){
        if (timeout) timeout--;
        else break;
    }
    for (i = 0; i < endpoints[number].rx_cnt; i++){
        buf[i] = endpoints[number].rx_buf[i];
    }
}

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

uint8_t _2wr = 0;

    if ((ep.rx_flag) && (ep.setup_flag)){
        if (packet -> bmRequestType == 0x80){ // standard device request (device to host)
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
SEND("UNK_DES");
_2wr = 1;
                        break;
                    }
                break;
                case GET_STATUS:
                    EP_WriteIRQ(0, (uint8_t *)&status, 2); // send status: Bus Powered
                break;
                default:
SEND("80:WR_REQ");
_2wr = 1;
                break;
            }
            //��� ��� �� �� ������� ������ � ���������� ������ ���������, �� �������������
            ep.status = SET_NAK_RX(ep.status);
            ep.status = SET_VALID_TX(ep.status);
        }else if(packet->bmRequestType == 0x00){ // standard device request (host to device)
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
SEND("0:WR_REQ");
_2wr = 1;
                break;
            }
            //���������� ������������� ������
            EP_WriteIRQ(0, (uint8_t *)0, 0);
            //��� ��� �� �� ������� ������ � ���������� ������ ���������, �� �������������
            ep.status = SET_NAK_RX(ep.status);
            ep.status = SET_VALID_TX(ep.status);
        }else if(packet -> bmRequestType == 0x02){ // standard endpoint request (host to device)
            if (packet->bRequest == CLEAR_FEATURE){
                //���������� ������������� ������
                EP_WriteIRQ(0, (uint8_t *)0, 0);
                //��� ��� �� �� ������� ������ � ���������� ������ ���������, �� �������������
                ep.status = SET_NAK_RX(ep.status);
                ep.status = SET_VALID_TX(ep.status);
            }else{
SEND("02:WR_REQ");
_2wr = 1;
            }
        }else if((packet->bmRequestType & VENDOR_MASK_REQUEST) == VENDOR_MASK_REQUEST){ // vendor request
SEND("vendor ");
            if(packet->bmRequestType & 0x80){ // read
SEND("read: ");
                uint8_t c = '?';
                EP_WriteIRQ(0, &c, 1);
            }else{ // write
SEND("write: ");
                EP_WriteIRQ(0, (uint8_t *)0, 0);
            }
printuhex(packet->wValue);
_2wr = 1;
            ep.status = SET_NAK_RX(ep.status);
            ep.status = SET_VALID_TX(ep.status);
        }else if((packet->bmRequestType & 0x7f) == CONTROL_REQUEST_TYPE){ // control request
_2wr = 1;
            //usb_LineCoding c;
            //uint8_t lbuf[10];
            //usb_cdc_notification *notif = (usb_cdc_notification*) lbuf;
            switch(packet->bRequest){
                case GET_LINE_CODING:
SEND("GET_LINE_CODING");
                    EP_WriteIRQ(0, (uint8_t*)&lineCoding, sizeof(lineCoding));
                break;
                case SET_LINE_CODING:
SEND("SET_LINE_CODING");
                    EP_Readx(0, (uint8_t*)&lineCoding);
printuhex(lineCoding.dwDTERate);
                    //EP_WriteIRQ(0, (uint8_t*)&lineCoding, sizeof(lineCoding));
                    //memcpy(&c, endpoints[0].rx_buf, sizeof(usb_LineCoding));
/*SEND("len: ");
printu(endpoints[0].rx_cnt);
printu(endpoints[0].rx_buf[6]);
SEND(", want baudrate: "); printuhex(c.dwDTERate);
SEND(", charFormat: "); printu(c.bCharFormat);
SEND(", parityType: "); printu(c.bParityType);
SEND(", dataBits: "); printu(c.bDataBits);*/
                break;
                case SET_CONTROL_LINE_STATE:
SEND("SET_CONTROL_LINE_STATE");
                    /*
                     * This Linux cdc_acm driver requires this to be implemented
                     * even though it's optional in the CDC spec, and we don't
                     * advertise it in the ACM functional descriptor.
                     */
                    /* We echo signals back to host as notification. *
                    notif->bmRequestType = CONTROL_REQUEST_TYPE | 0x80;
                    notif->bNotificationType = SET_LINE_CODING;
                    notif->wValue = 0;
                    notif->wIndex = 0;
                    notif->wLength = 2;
                    lbuf[8] = packet->wValue & 3;
                    lbuf[9] = 0;
                    EP_WriteIRQ(3, lbuf, 10);*/
                    //EP_WriteIRQ(0, (uint8_t *)0, 0);
                break;
                case SEND_BREAK:
SEND("SEND_BREAK");
                break;
                default:
SEND("undef control req");
            }
            if((packet->bmRequestType & 0x80) == 0) EP_WriteIRQ(0, (uint8_t *)0, 0); // write acknowledgement
            ep.status = SET_NAK_RX(ep.status);
            ep.status = SET_VALID_TX(ep.status);
        }
    } else if (ep.rx_flag){    //������������ ������ ������
        //��� ��� ������������ �� ����� ��������� ����������
        //�� ���������� DTOG�
        ep.status = CLEAR_DTOG_RX(ep.status);
        ep.status = CLEAR_DTOG_TX(ep.status);
        //��� ��� �� ������� ����� ������ �� �����, �������������
        ep.status = SET_VALID_RX(ep.status);
        ep.status = SET_STALL_TX(ep.status);
    } else if (ep.tx_flag){    //�������� �������� ������
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
if(_2wr){
    usart_putchar(' ');
    if (ep.rx_flag) usart_putchar('r');
    else usart_putchar('t');
    printu(packet->wLength);
    if(ep.setup_flag) usart_putchar('s');
    usart_putchar(' ');
    usart_putchar('R');
    printu(packet->bRequest);
    usart_putchar('V');
    printu(packet->wValue);
    usart_putchar('T');
    printu(packet->bmRequestType);
    usart_putchar('\n');
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
