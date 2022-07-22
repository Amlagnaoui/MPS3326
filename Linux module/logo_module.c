#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/errno.h>
static int i=0;
static int maxval=250;
int ret= -1;
struct device *my_dev;


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Amine LAGNAOUI");
MODULE_DESCRIPTION("LED logo driver");


#define DRIVER_NAME "LOGO"
#define DRIVER_CLASS "LOGOClass"

static struct i2c_adapter * LOGO_adapter = NULL;
static struct i2c_client * LOGO_client = NULL;

#define I2C_BUS_AVAILABLE 	1
#define SLAVE_DEVICE_NAME	"LOGO"
#define LOGO_SLAVE_ADDRESS	0x30		//Driver's address defined in Datasheet







static const struct i2c_device_id logo_id[] = {
		{SLAVE_DEVICE_NAME, 0}
};


static struct i2c_driver logo_driver = {
		.driver = {
			.name = SLAVE_DEVICE_NAME,
			.owner = THIS_MODULE
			}
};

static struct i2c_board_info logo_i2c_board_info = {
	I2C_BOARD_INFO(SLAVE_DEVICE_NAME, LOGO_SLAVE_ADDRESS)
};

/*Variables for device and device class*/

static dev_t myDeviceNr;
static struct class *myClass;
static struct cdev myDevice;

static void I2C_CurrentDimAll(u8 a)		
{
	
	//s32 *Value;
	for(i=0;i<16;i++)
	{
		i2c_smbus_write_byte_data(LOGO_client,(0x0A +3*i),a);			//write current registers
	}
}

static void delay(long long e){
	int i;
	for (i=0;i<e;i++){
			//do nothing
		} 
}



static void I2C_PWMDimAll(int pwmval)		//Dimming all LEDs at once
{
  s32 PWM_L = 0, PWM_M = 0;
  PWM_L = pwmval & 0x000F;
  PWM_M = pwmval >> 4;


  for (i=0;i<16;i++)
  {
		i2c_smbus_write_byte_data(LOGO_client,(0x0B +3*i),PWM_L);
		i2c_smbus_write_byte_data(LOGO_client,(0x0C +3*i),PWM_M);
  }
}

static void I2C_PWMDimOne(int pwmval,int i)	//Dimming one LEDs Channel with i is (Channel number - 1)
{
s32 PWM_L = 0, PWM_M = 0;
PWM_L = pwmval & 0x000F;
PWM_M = pwmval >> 4;

i2c_smbus_write_byte_data(LOGO_client,(0x0B +3*i),PWM_L);
i2c_smbus_write_byte_data(LOGO_client,(0x0C +3*i),PWM_M);
}

static void I2C_M1027_Setup(void)
{
	i2c_smbus_write_byte_data(LOGO_client,(0x01),0x01);
	i2c_smbus_write_byte_data(LOGO_client,(0x04),0xFF);
	i2c_smbus_write_byte_data(LOGO_client,(0x05),0xFF);

  I2C_CurrentDimAll(63);
  I2C_PWMDimAll(0);
}

//##############################################################################################################################################################################################################

/** @brief Write handler for the maxvalue sysfs attribute
*
* First use of attributes and trying to familiarize with them in full autonomy so excuse any school boy error that may be found and please address it if possible
*
*
*/
static ssize_t logo_max_value_write(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
    struct i2c_client *client;
    int size = 0;

    client = to_i2c_client(dev);
	
    int i,j;
		/*for(j=0;j<3;j++)
		{
			for(i=0;i<200;i++)
			{
				I2C_PWMDimAll(i);
			}
			for(i=200;i>0;i--)
			{
				I2C_PWMDimAll(i);
			}
		}*/
		
		for(j=0;j<10;j++)
		{
			for(i=0;i<45;i++)
			{
				I2C_PWMDimOne(i,3);
				I2C_PWMDimOne((45-i),8);
			}
			for(i=maxval;i>0;i--)
			{
				I2C_PWMDimOne(i,3);
				I2C_PWMDimOne((45-i),8);
			}
		}
for(j=0;j<10;j++)
		{
			for(i=0;i<45;i++)
			{
				I2C_PWMDimOne(i,3);
				I2C_PWMDimOne(i,8);
			}
			for(i=maxval;i>0;i--)
			{
				I2C_PWMDimOne(i,3);
				I2C_PWMDimOne(i,8);
			}
		}
		delay(100);
		I2C_PWMDimAll(0);
		printk("LOGO: Done \n");
		printk("LOGO: Write called !\n");
    return count;
}

/** @brief Read handler for the tx_address sysfs attribute */
static ssize_t logo_max_value_read(struct device *dev, struct device_attribute *attr, char *buf) {

	struct i2c_client *client;
    int size = 0;
    client = to_i2c_client(dev);
	printk("LOGO: Read called !\n");
	int i,j;
		
		for(j=0;j<30;j++)
		{
			for(i=0;i<40;i++)
			{
				I2C_PWMDimOne(i,3);
				I2C_PWMDimOne((40-i),8);
			}
			for(i=40;i>0;i--)
			{
				I2C_PWMDimOne(i,3);
				I2C_PWMDimOne((40-i),8);
			}
		}
for(j=0;j<10;j++)
		{
			for(i=0;i<40;i++)
			{
				I2C_PWMDimOne(i,3);
				I2C_PWMDimOne(i,8);
			}
			for(i=40;i>0;i--)
			{
				I2C_PWMDimOne(i,3);
				I2C_PWMDimOne(i,8);
			}
		}
		delay(100);
		I2C_PWMDimAll(0);
		printk("LOGO: Done \n");
    return 4;
}


/** @brief Device attribute for configuring the tx address */
static DEVICE_ATTR(maxval, 0644, logo_max_value_read, logo_max_value_write);    //Equivalent to declaring a struct of type device_attribute
                                                                                //dev_attr_maxval

										//Encountered probs => Even when changing the access rights nothing, file cannot be opened



/** @brief An array containing all the sysfs attributes */
static struct attribute *logo_attributes[] = {
    &dev_attr_maxval.attr,
    NULL,
};


/** @brief The attribute group for creating and destroying all the sysfs attributes at once instead of one at a time */
static struct attribute_group logo_attribute_group = {
    .attrs = logo_attributes
};


//############################################################################################################################################################################################################
//
/*Declarations and what not*/ 


/**
 * @brief This function is called, when the device file is opened*/


static int driver_open(struct inode *deviceFile, struct file *instance) {
	printk("LOGO: MyDeviceDriver -  Open was called\n");
	return 0;
}

/** @brief This function is called, when the device file is close
 */
static int driver_close(struct inode *deviceFile, struct file *instance) {
	printk("LOGO: MyDeviceDriver -  Close was called\n");
	return 0;
}
//############################################################################################################################################################################################################

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
};


static int __init MyFirstModule_init (void)
{
	
	int err;
	//Lets call it init
    pr_info("Here I come !!!!!!!!!!!!!!!!!! \n");
	u8 id;
	if ( alloc_chrdev_region(&myDeviceNr, 0, 1, DRIVER_NAME) < 0){
		printk("LOGO: Device Nr. could not be allocated ! \n");
	}
	printk("LOGO: MyDeviceDriver - Device Nr %d was registered \n",myDeviceNr);
	
	if((myClass = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		printk("LOGO: Device class can not be created!\n");
        goto ClassError;
	}
	if(device_create(myClass, NULL, myDeviceNr, NULL, DRIVER_NAME) == NULL){
		printk("LOGO: Device file cannot be created ! \n");
        goto FileError;
    }
	
	cdev_init(&myDevice, &fops);
	
	//Linking to the device
	//err = sysfs_create_group(&spi->dev.kobj, &nrf905_attribute_group);
	my_dev = root_device_register("my_logo");
	printk("LOGO: BD !\n");
	err = sysfs_create_group(&my_dev->kobj, &logo_attribute_group);
	printk("LOGO: error = %x",err);
	printk("LOGO: AD !\n");

	if(cdev_add(&myDevice, myDeviceNr, 1) == -1){
		printk("LOGO: Device registration failed ! \n");
        goto KernelError;
    }
	
	LOGO_adapter = i2c_get_adapter(I2C_BUS_AVAILABLE);
	
	if(LOGO_adapter != NULL) {
		LOGO_client = i2c_new_client_device(LOGO_adapter, &logo_i2c_board_info);
		if(LOGO_client != NULL) {
			if(i2c_add_driver(&logo_driver) != -1) {
				ret = 0;
			}
			else
				printk("LOGO: Can't add driver...\n");
		}
		i2c_put_adapter(LOGO_adapter);
	}
	printk("LOGO: LOGO Driver added succefully!\n");
	
	
	//Simple welcoming sequence with the LEDs 
	
        I2C_M1027_Setup();
        printk("LOGO: Everything setup \n");
		printk("LOGO: Maximum value is %d \n",maxval);
        int i,j;
		/*for(j=0;j<3;j++)
		{
			for(i=0;i<200;i++)
			{
				I2C_PWMDimAll(i);
			}
			for(i=200;i>0;i--)
			{
				I2C_PWMDimAll(i);
			}
		}*/
		for(j=0;j<10;j++)
		{
			for(i=0;i<maxval;i++)
			{
				I2C_PWMDimOne(i,3);
				I2C_PWMDimOne((maxval-i),8);
			}
			for(i=maxval;i>0;i--)
			{
				I2C_PWMDimOne(i,3);
				I2C_PWMDimOne((maxval-i),8);
			}
		}
for(j=0;j<10;j++)
		{
			for(i=0;i<maxval;i++)
			{
				I2C_PWMDimOne(i,3);
				I2C_PWMDimOne(i,8);
			}
			for(i=maxval;i>0;i--)
			{
				I2C_PWMDimOne(i,3);
				I2C_PWMDimOne(i,8);
			}
		}
		delay(100);
		I2C_PWMDimAll(0);
		printk("LOGO: Done \n");
        return ret;


	
	KernelError:
		device_destroy(myClass, myDeviceNr);
		printk("LOGO: Destroy 1 called ! \n");
	FileError:
		class_destroy(myClass);
		printk("LOGO: Destroy 2 called ! \n");
	ClassError:
		unregister_chrdev(myDeviceNr, DRIVER_NAME);
		printk("LOGO: Destroy 3 called ! \n");
		return (-1);
}



static void __exit MyFirstModule_exit(void) {
	printk("LOGO: MyDeviceDriver - Goodbye, Kernel!\n");
	sysfs_remove_group(&myDevice.kobj, &logo_attribute_group);
	if (my_dev)
            root_device_unregister(my_dev);
	i2c_unregister_device(LOGO_client);
	i2c_del_driver(&logo_driver);
	cdev_del(&myDevice);
	
	//Remove attribute
	
	device_destroy(myClass, myDeviceNr);
	class_unregister(myClass);
	class_destroy(myClass);
	unregister_chrdev_region(myDeviceNr, 1);printk("LOGO: Destroy 3 called ! \n");
	
	printk("Exiting done ! \n");
}

MODULE_PARM_DESC(maxval,"Maximum Value");
module_param(maxval, int, 0);
module_init(MyFirstModule_init);
module_exit(MyFirstModule_exit);








