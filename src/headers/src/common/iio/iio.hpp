#if !defined(_COMMON_IIO_IIO_HPP_)
#define _COMMON_IIO_IIO_HPP_

#include "gpio.hpp"

#include "baseband.hpp"
#include "iioConfig.hpp"

#include <string.h>

class IIO
{
private:
    GPIO *gpio;

	iodev type;
	struct iio_context *context   = NULL;
    struct iio_device *device = NULL;   // Streaming devices
    struct iio_channel *channel0 = NULL;
    struct iio_buffer  *buffer = NULL;
	int bufferSize;
    stream_cfg config;


    static volatile IIO *instanceTX;
    static pthread_mutex_t mtxTX;

	static volatile IIO *instanceRX;
    static pthread_mutex_t mtxRX;
public:
    IIO(/* args */);
    ~IIO();

    static volatile IIO *getInstanceTX(); 
	static volatile IIO *getInstanceRX();

    void init(double freq, double gain);
    void close();

	int send(uint8_t *databuffer);
	int recv(uint8_t *databuffer);
	uint8_t *recv();


	bool setTXGain(double gain);

    int requestToSend();

};

volatile IIO * IIO::instanceTX = NULL;
pthread_mutex_t IIO::mtxTX;

volatile IIO * IIO::instanceRX = NULL;
pthread_mutex_t IIO::mtxRX;

IIO::IIO(/* args */)
{
	this->gpio = (GPIO *)GPIO::getInstance();
}

IIO::~IIO()
{
}

volatile IIO *IIO::getInstanceTX()
{
    if (NULL == instanceTX)
    {
        pthread_mutex_lock(&mtxTX);
        if (NULL == instanceTX)  
        {
            instanceTX = new IIO();
			instanceTX->type = TX;
        }
        pthread_mutex_unlock(&mtxTX);
    }
    IIO_DEBUG("IIO_DEBUG: Get the %sIIO instanceTX: %d%s\n",GREEN, instanceTX,NONE);

    return instanceTX;
}

volatile IIO *IIO::getInstanceRX()
{
    if (NULL == instanceRX)
    {
        pthread_mutex_lock(&mtxRX);
        if (NULL == instanceRX)  
        {
            instanceRX = new IIO();
			instanceRX->type = RX;
        }
        pthread_mutex_unlock(&mtxRX);
    }
    IIO_DEBUG("IIO_DEBUG: Get the %sIIO instanceRX: %d%s\n",GREEN, instanceRX,NONE);

    return instanceRX;
}


void IIO::init(double freq, double gain)
{
	this->type = type;

    config.lo_hz = MHZ((freq)); // 2.5 GHz rf frequency 401.69
    config.tx_gain = gain; //发送衰减，单位：dB，范围：-89.75~0，精度：0.25dB 
	config.bw_hz = MHZ(20); // 1.5 MHz rf bandwidth
	config.fs_hz = MHZ(10);   // 2.5 MS/s tx sample rate
	//txcfg.rfport = "A"; // port A (select for rf freq.)

    IIO_DEBUG("* Acquiring IIO context\n");
	IIO_ASSERT((context = iio_create_default_context()) && "No context");

	IIO_DEBUG("* Acquiring AD9361 streaming devices\n");
	IIO_ASSERT(get_ad9361_stream_dev(context, type, &device) && "No tx dev found");

	IIO_DEBUG("* Configuring AD9361 for streaming\n");
	IIO_ASSERT(cfg_ad9361_streaming_ch(context, &config, type, 0) && "TX port 0 not found");

	IIO_DEBUG("* Initializing AD9361 IIO streaming channels\n");
	IIO_ASSERT(get_ad9361_stream_ch(context, type, device, 0, &channel0) && "TX chan  not found");

	IIO_DEBUG("* Enabling IIO streaming channels\n");
	iio_channel_enable(channel0);

	IIO_DEBUG("* Creating non-cyclic IIO buffers with 1 MiS\n");

	buffer = iio_device_create_buffer(device, 512, false);
	if (!buffer) {
		perror("Could not create TX buffer");
		close();
	}
}

int IIO::send(uint8_t *databuffer)
{
	if(this->type)
	{
		static uint8_t 	*bufferHead;		// iio buffer head
		static uint8_t 	*bufferTail;		// iio buffer tail
		static ptrdiff_t bufferStep;		// iio ???
		static ssize_t 	 sendBytes;

		requestToSend();

		bufferStep = iio_buffer_step(buffer);
		bufferHead = (uint8_t *)iio_buffer_first(buffer,channel0);
		bufferTail = (uint8_t *)iio_buffer_end(buffer);

		memcpy(bufferHead, databuffer, 2048);

		sendBytes = iio_buffer_push(buffer);	//send

		IIO_DEBUG("-IIO_DEBUG: send bytes:[%d]\n", sendBytes);

		return sendBytes;
	}

	return 0;
}

int IIO::recv(uint8_t *databuffer)
{
	if(!this->type)
	{
		char *bufferHead;		// iio buffer head
		uint8_t *bufferTail;		// iio buffer tail
		ptrdiff_t bufferStep;		// iio ???
		int recvBytes;

		do{
			recvBytes = iio_buffer_refill(buffer);
			if(recvBytes < 0)
			{IIO_DEBUG("-IIO_DEBUG: buffer refill:[%d]\n", recvBytes);}	
		} while (0 >= recvBytes);

		bufferStep = iio_buffer_step(buffer);
		bufferHead = (char *)iio_buffer_first(buffer,channel0);
		bufferTail = (uint8_t *)iio_buffer_end(buffer);

		memcpy(databuffer, bufferHead, 2048);

		IIO_DEBUG("-IIO_DEBUG: recv bytes:[%d]\n", recvBytes);

		return recvBytes;
	}
	
	return 0;
}

uint8_t *IIO::recv()
{
	if(!this->type)
	{
		uint8_t *bufferHead;		// iio buffer head
		uint8_t *bufferTail;		// iio buffer tail
		ptrdiff_t bufferStep;		// iio ???
		int recvBytes;

		do{
			recvBytes = iio_buffer_refill(buffer);
			if(recvBytes < 0)
			{IIO_DEBUG("-IIO_DEBUG: buffer refill:[%d]\n", recvBytes);}	
		} while (0 >= recvBytes);

		bufferStep = iio_buffer_step(buffer);
		bufferHead = (uint8_t *)iio_buffer_first(buffer,channel0);
		bufferTail = (uint8_t *)iio_buffer_end(buffer);

		IIO_DEBUG("-IIO_DEBUG: recv bytes:[%d]\n", recvBytes);

		return bufferHead;
	}
	
	return 0;
}


bool IIO::setTXGain(double gain)
{
	struct iio_channel *channel = NULL;

	if (!get_phy_chan(context, type, 0, &channel)) 
	{ 
		IIO_DEBUG("IIO_DEBUG: %sset gain error: Acquiring AD9361 phy channel 0 faild%s\n", RED, NONE);
		return false; 
	}

	if(this->type) 
    {
		if(0 == wr_ch_lli(channel, "hardwaregain", gain))
		{IIO_DEBUG("IIO_DEBUG: set TX gain %f dB.\n", gain);}; 
	   	return true;
    }

	return false; 
}

int IIO::requestToSend()
{
	while (0 != gpio->get(985));

	return 1;
}

void IIO::close()
{
    IIO_MSG("IIO_MSG: Destroying buffers\n");
	if (buffer) { iio_buffer_destroy(buffer); }

	IIO_MSG("IIO_MSG: Disabling streaming channels\n");
	if (channel0) { iio_channel_disable(channel0); }

	IIO_MSG("IIO_MSG: Destroying context\n");
	if (context) { iio_context_destroy(context); }
}

#endif // _COMMON_IIO_IIO_HPP_
