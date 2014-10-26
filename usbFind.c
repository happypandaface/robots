#include <stdio.h>
#include <usb.h>

int main(int argc, char **argv)
{
	struct usb_bus *busses;
	usb_init();
	usb_find_busses();
	usb_find_devices();

	busses = usb_get_busses();

	struct usb_bus *bus;
	int c, i, a;

	for (bus = busses; bus; bus = bus->next)
	{
		struct usb_device *dev;
		printf("usb found\n");
		for (dev = bus->devices; dev; dev = dev->next)
		{
			if (dev->descriptor.idVendor == 0x05e1 && dev->descriptor.idProduct == 0x0408)
			{
				printf("Its the thing!\n");
				struct usb_dev_handle *handle = usb_open(dev);
				int err;
				err = usb_claim_interface(handle, 0);
				if (err < 0)
					printf("err on claim\n");
				int numBytes = 1024;
				char bytes[numBytes];
				err = usb_interrupt_read(handle, numBytes, bytes, numBytes, 1000);
				if (err < 0)
					printf("err on interrupt read\n");
				err = usb_bulk_read(handle, numBytes, bytes, numBytes, 1000);
				if (err < 0)
					printf("err on bulk read\n");
				printf("got: %s\n", bytes);
				err = usb_release_interface(handle, 0);
				if (err < 0)
					printf("err on release\n");
				err = usb_close(handle);
				if (err < 0)
					printf("err on close\n");
			}
		}
	}
	return 0;
}
