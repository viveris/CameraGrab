/*
* MIT License
*
* Copyright (c) 2021 Viveris Technologies
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is 
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
/**
 * @file  CameraGrab.c
 * @brief Application to take camera snapshot.
 * @author Sebastien CORBEAU <sebastien.corbeau@viveris.fr>
 */
#include <opencv2/opencv.hpp>

#include <stdio.h>

#include "DeviceEnumerator.h"
#include "Version.h"

/*
 * @brief Output file default name.
 */
#define DEFAULT_OUTPUT_FILE	"output"

/*
 * @brief Output file default format.
 */
#define DEFAULT_EXT ".jpg"

/*
 * @brief Application name.
 */
#define APPLICATION_NAME	"CameraGrab.exe"

/*
 * @brief Is list devices is required by user.
 */
static bool g_list_dev = false;

/*
 * @brief Image filename extension.
 */
static std::string g_extension = "";

/*
 * @brief Output filename.
 */
static std::string g_output = DEFAULT_OUTPUT_FILE;

/*
 * @brief Image width (in pixel).
 */
static int g_width = 640;

/*
 * @brief Image height (in pixel).
 */
static int g_height = 480;

/*
 * @brief Index of device.
 */
static int g_device = 0;

/*
 * @brief Focus of camera (0 to 255).
 */
static int g_focus = -1;

/*
 * @brief Display help application.
 * @return None.
 */
static void printUsage(void)
{
	printf("%s <option> <parameter>\r\n", APPLICATION_NAME);
	printf("options :\r\n");
	printf("\t[/h|/help]: Display this help.\r\n");
	printf("\t[/d|/device]: Index of capture device.\r\n");
	printf("\t[/o|/output file]: File to save camera picture.\r\n");
	printf("\t[/f|/focus focus]: Set focus value (0 to 255; -1 for autofocus).\r\n");
	printf("\t[/F|/format <bmp|jpg|png>] : Specify output format.\r\n");
	printf("\t[/l|/list-device]: List availables devices.\r\n");
	printf("\t[/W|/width]: Width of capture device.\r\n");
	printf("\t[/H|/height]: Height of capture device.\r\n");
	printf("\t[/v|/version]: Display application version.\r\n");
}

/*
 * @brief Display help application.
 * @return None.
 */
static void printVersion(void)
{
	printf("%s Version %d.%d.%d\r\n",
		APPLICATION_NAME,
		VERSION_MAJOR,
		VERSION_MINOR,
		VERSION_PATCH);
}

/*
 * @brief Display help application.
 * @param[in] argc  Number of argument.
 * @param[in] argv  Arguments value.
 * @return 0 when all parameters are parsed,
 *         1 when command was executed (version or help),
 *         -1 on parsing error.
 */
static int parseOpts(int argc, char* argv[])
{
	int ret = 0;

	int n = 1;
	while ((n < argc) && (ret == 0)) {
		if (!strcmp(argv[n], "/h") || !strcmp(argv[n], "/help"))
		{
			printUsage();
			ret = 1;
		}
		else if (!strcmp(argv[n], "/v") || !strcmp(argv[n], "/version"))
		{
			printVersion();
			ret = 1;
		}
		else if (!strcmp(argv[n], "/d") || !strcmp(argv[n], "/device"))
		{
			if (++n < argc)
			{
				g_device = atoi(argv[n]);
				if (g_device < 0)
				{
					printf("\"%s\" is not a valid device.\r\n", argv[n]);
					ret = -1;
				}
			}
			else
			{
				printf("Invalid usage /h or /help to display help\r\n");
				ret = -1;
			}
		}
		else if (!strcmp(argv[n], "/o") || !strcmp(argv[n], "/output"))
		{
			if (++n < argc)
			{
				g_output = argv[n];
			}
			else
			{
				printf("Invalid usage /h or /help to display help\r\n");
				ret = -1;
			}
		}
		else if (!strcmp(argv[n], "/f") || !strcmp(argv[n], "/focus"))
		{
			if (++n < argc)
			{
				g_focus = atoi(argv[n]);
				if (g_focus != -1)
				{
					if (g_focus < -1 || g_focus > 255)
					{
						printf("\"%s\" is not a valid focus.\r\n", argv[n]);
						ret = -1;
					}
					else if (g_focus % 5)
					{
						printf("\"%s\" must be multiple of 5.\r\n", argv[n]);
						ret = -1;
					}
				}
			}
			else
			{
				printf("Invalid usage /h or /help to display help\r\n");
				ret =-1;
			}
		}
		else if (!strcmp(argv[n], "/F") || !strcmp(argv[n], "/format"))
		{
			if (++n < argc)
			{
				if (!strcmp(argv[n], "bmp") || !strcmp(argv[n], "BMP"))
				{
					g_extension = ".bmp";
				}
				else if (!strcmp(argv[n], "jpg") || !strcmp(argv[n], "JPG"))
				{
					g_extension = ".jpg";
				}
				else if (!strcmp(argv[n], "png") || !strcmp(argv[n], "PNG"))
				{
					g_extension = ".png";
				}
				else
				{
					printf("\"%s\" is unknown extension\r\n", argv[n]);
					ret =-1;
				}
			}
			else
			{
				printf("Invalid usage -h or --help to display help\r\n");
				ret =-1;
			}
		}
		else if (!strcmp(argv[n], "/l") || !strcmp(argv[n], "/list-device"))
		{
			g_list_dev = true;
		}
		else if (!strcmp(argv[n], "/H") || !strcmp(argv[n], "/height"))
		{
			if (++n < argc)
			{
				g_height = atoi(argv[n]);
				if (g_height <= 0)
				{
					printf("\"%s\" is not a valid height.\r\n", argv[n]);
					ret =-1;
				}
			}
			else
			{
				printf("Invalid usage /h or /help to display help\r\n");
				ret =-1;
			}
		}
		else if (!strcmp(argv[n], "/W") || !strcmp(argv[n], "/width"))
		{
			if (++n < argc)
			{
				g_width = atoi(argv[n]);
				if (g_width <= 0)
				{
					printf("\"%s\" is not a valid width.\r\n", argv[n]);
					ret =-1;
				}
			}
			else
			{
				printf("Invalid usage /h or /help to display help\r\n");
				ret =-1;
			}
		}
		else
		{
			printf("Invalid option /h or /help to display help\r\n");
			ret =-1;
		}
		n++;
	}

	return ret;
}

/*
 * @brief Application entry point.
 * @param[in] argc  Number of argument.
 * @param[in] argv  Arguments value.
 * @return 0 in case of success otherwise -1.
 */
int main(int argv, char** argc)
{
	cv::VideoCapture* camera = NULL;
	int height, width;
	int autofocus;
	int ret = 0;

	ret = parseOpts(argv, argc);
	if(-1 == ret)
	{
		return -1;
	}
	else if(1 == ret)
	{
		return 0;
	}

#ifdef _DEBUG
	printf("==== DEBUG ==== \r\n\r\n");
	if (g_list_dev)
	{
		printf("Force device list.\r\n");
	}
	else
	{
		printf("Device index : %d\r\n", g_device);
		printf("Ouput file : %s\r\n", g_output.c_str());
		printf("Extension : %s\r\n", g_extension.c_str());
		printf("Width : %d\r\n", g_width);
		printf("Height : %d\r\n", g_height);
		printf("Focus : %d(%s)\r\n", g_focus, 
									(g_focus == -1) ? "Autofocus" : "Manual");
	}
	printf("==== END DEBUG ==== \r\n\r\n");
	fflush(stdout);
#endif

	if (g_list_dev)
	{
		// Video Devices
		DeviceEnumerator de;
		std::map<int, Device> devices = de.getVideoDevicesMap();

		// Print information about the devices
		for (auto const& device : devices) {
			printf("== VIDEO DEVICE (id:%d) ==\r\n", device.first);
			printf(" * Name: %s\r\n", device.second.deviceName.c_str());
			printf(" * Path: %s\r\n", device.second.devicePath.c_str());

			camera = new cv::VideoCapture(device.first);
			if (!camera->isOpened()) {
				printf(" * ERROR: Could not open camera\r\n");
			}

			/* Set a high resoltuion to get max camera resolution */
			camera->set(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH,
						10000);
			camera->set(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT, 
						10000);

			width = static_cast<int>(
				camera->get(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH));
			height = static_cast<int>(
				camera->get(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT));
			autofocus = static_cast<int>(
				camera->get(cv::VideoCaptureProperties::CAP_PROP_AUTOFOCUS));
			
			printf(" * Max width : %d\r\n", width);
			printf(" * Max height : %d\r\n", height);
			printf(" * Autofocus : %d\r\n", autofocus);


			delete camera;
			printf("\r\n");
			ret = 0;
		}
	}
	else
	{
		camera = new cv::VideoCapture(g_device, cv::CAP_DSHOW);

		if (!camera->isOpened())
		{
			printf("Failed to open camera.\r\n");
		}
		else
		{
			cv::Mat frame;
			
			/* Set camera resolution  */
			if (!camera->set(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH,
							 g_width))
			{
				printf("Failed to set Width to %d pixel\r\n", g_width);
				ret = -1;
			}
			if (!camera->set(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT,
							 g_height))
			{
				printf("Failed to set Height to %d pixel\r\n", g_height);
				ret = -1;
			}

			/* Set focus */
			if (g_focus != -1)
			{
				printf("Configure manual focus : %d\r\n", g_focus);
				if (!camera->set(cv::VideoCaptureProperties::CAP_PROP_FOCUS,
								 g_focus))
				{
					printf("Failed to configure manuel focus\r\n");
					ret = -1;
				}
			}
#if 0
			else
			{
				printf("Configure autofocus\r\n");
				if (!camera->set(cv::VideoCaptureProperties::CAP_PROP_AUTOFOCUS,
								 1))
				{
					printf("Failed to configure autofocus\r\n");
					ret = -1;
				}
			}
#endif
			
			if(ret == 0)
			{
				if (g_extension.compare(""))
				{
					g_output += g_extension;
				}
				else
				{
					g_output += DEFAULT_EXT;
				}

#ifdef _DEBUG
				printf("Output filename : %s\r\n", g_output.c_str());
#endif
				if (!camera->read(frame))
				{
					printf("Failed to read camera frame\r\n");
					ret = -1;
				}
				else if (!cv::imwrite(g_output, frame))
				{
					printf("Failed to save camera frame to %s\r\n",
						g_output.c_str());
					ret = -1;
				}
				else
				{
					printf("Camera frame save to %s\r\n", g_output.c_str());
				}
			}

			delete camera;
		}
	}

	return ret;
}
