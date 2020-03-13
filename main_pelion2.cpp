// // ----------------------------------------------------------------------------
// // Copyright 2016-2019 ARM Ltd.
// //
// // SPDX-License-Identifier: Apache-2.0
// //
// // Licensed under the Apache License, Version 2.0 (the "License");
// // you may not use this file except in compliance with the License.
// // You may obtain a copy of the License at
// //
// //     http://www.apache.org/licenses/LICENSE-2.0
// //
// // Unless required by applicable law or agreed to in writing, software
// // distributed under the License is distributed on an "AS IS" BASIS,
// // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// // See the License for the specific language governing permissions and
// // limitations under the License.
// // ----------------------------------------------------------------------------
// #ifndef MBED_TEST_MODE
// #include "mbed.h"
// #include "kv_config.h"
// #include "mbed-cloud-client/MbedCloudClient.h" // Required for new MbedCloudClient()
// #include "factory_configurator_client.h"       // Required for fcc_* functions and FCC_* defines
// #include "m2mresource.h"                       // Required for M2MResource

// #include "mbed-trace/mbed_trace.h" // Required for mbed_trace_*
// #include "ADE7953.h"

// // Pointers to the resources that will be created in main_application().
// static MbedCloudClient *cloud_client;
// static bool cloud_client_running = true;
// static NetworkInterface *network = NULL;

// // Fake entropy needed for non-TRNG boards. Suitable only for demo devices.
// const uint8_t MBED_CLOUD_DEV_ENTROPY[] = {0xf6, 0xd6, 0xc0, 0x09, 0x9e, 0x6e, 0xf2, 0x37, 0xdc, 0x29, 0x88, 0xf1, 0x57, 0x32, 0x7d, 0xde, 0xac, 0xb3, 0x99, 0x8c, 0xb9, 0x11, 0x35, 0x18, 0xeb, 0x48, 0x29, 0x03, 0x6a, 0x94, 0x6d, 0xe8, 0x40, 0xc0, 0x28, 0xcc, 0xe4, 0x04, 0xc3, 0x1f, 0x4b, 0xc2, 0xe0, 0x68, 0xa0, 0x93, 0xe6, 0x3a};

// static M2MResource *m2m_get_res;
// static M2MResource *intelligentElectricBox;
// static M2MResource *m2m_put_res;
// static M2MResource *m2m_post_res;
// static M2MResource *m2m_deregister_res;

// void print_client_ids(void)
// {
//     printf("Account ID: %s\n", cloud_client->endpoint_info()->account_id.c_str());
//     printf("Endpoint name: %s\n", cloud_client->endpoint_info()->internal_endpoint_name.c_str());
//     printf("Device Id: %s\n\n", cloud_client->endpoint_info()->endpoint_name.c_str());
// }

// void button_press(void)
// {
//     m2m_get_res->set_value(m2m_get_res->get_value_int() + 1);
//     printf("Counter %" PRIu64 "\n", m2m_get_res->get_value_int());
// }

// void put_update(const char * /*object_name*/)
// {
//     printf("PUT update %d\n", (int)m2m_put_res->get_value_int());
// }

// void execute_post(void * /*arguments*/)
// {
//     printf("POST executed\n");
// }

// void deregister_client(void)
// {
//     printf("Unregistering and disconnecting from the network.\n");
//     cloud_client->close();
// }

// void deregister(void * /*arguments*/)
// {
//     printf("POST deregister executed\n");
//     m2m_deregister_res->send_delayed_post_response();

//     deregister_client();
// }

// void client_registered(void)
// {
//     printf("Client registered.\n");
//     print_client_ids();
// }

// void client_unregistered(void)
// {
//     printf("Client unregistered.\n");
//     (void)network->disconnect();
//     cloud_client_running = false;
// }

// void client_error(int err)
// {
//     printf("client_error(%d) -> %s\n", err, cloud_client->error_description());
// }

// void update_progress(uint32_t progress, uint32_t total)
// {
//     uint8_t percent = (uint8_t)((uint64_t)progress * 100 / total);
//     printf("Update progress = %" PRIu8 "%%\n", percent);
// }

// extern "C" MBED_WEAK void dispatch_host_command(int);

// int main(void)
// {
//     int status;

//     status = mbed_trace_init();
//     if (status != 0)
//     {
//         printf("mbed_trace_init() failed with %d\n", status);
//         return -1;
//     }

//     // Mount default kvstore
//     printf("Application ready\n");
//     status = kv_init_storage_config();
//     if (status != MBED_SUCCESS)
//     {
//         printf("kv_init_storage_config() - failed, status %d\n", status);
//         return -1;
//     }

//     // Connect with NetworkInterface
// #include "CellularExternalControl.h"
//     printf("Connect to network\n");
//     CellularExt_HWReset_Set(1);
//     CellularExt_PreInitModule();
//     // CellularExt_Attach();
//     do{
//         network = NetworkInterface::get_default_instance();
//         if (network == NULL)
//         {
//             printf("Failed to get default NetworkInterface\n");
//             return -1;
//         }
//         status = network->connect();
//         if (status != NSAPI_ERROR_OK)
//         {
//             printf("NetworkInterface failed to connect with %d\n", status);
//             return -1;
//         }
//     } while (status != NSAPI_ERROR_OK);

//         printf("Network initialized, connected with IP %s\n\n", network->get_ip_address());

//     // Run developer flow
//     printf("Start developer flow\n");
//     status = fcc_init();
//     if (status != FCC_STATUS_SUCCESS)
//     {
//         printf("fcc_init() failed with %d\n", status);
//         return -1;
//     }

//     // Inject hardcoded entropy for the device. Suitable only for demo devices.
//     (void)fcc_entropy_set(MBED_CLOUD_DEV_ENTROPY, sizeof(MBED_CLOUD_DEV_ENTROPY));
//     status = fcc_developer_flow();
//     if (status != FCC_STATUS_SUCCESS && status != FCC_STATUS_KCM_FILE_EXIST_ERROR && status != FCC_STATUS_CA_ERROR)
//     {
//         printf("fcc_developer_flow() failed with %d\n", status);
//         return -1;
//     }

//     printf("Create resources\n");
//     M2MObjectList m2m_obj_list;

//     // GET resource 3200/0/5501
//     m2m_get_res = M2MInterfaceFactory::create_resource(m2m_obj_list, 3200, 0, 5501, M2MResourceInstance::INTEGER, M2MBase::GET_ALLOWED);
//     if (m2m_get_res->set_value(0) != true)
//     {
//         printf("m2m_get_res->set_value() failed\n");
//         return -1;
//     }

//     intelligentElectricBox = M2MInterfaceFactory::create_resource(m2m_obj_list, 3200, 0, 5502, M2MResourceInstance::STRING, M2MBase::GET_PUT_ALLOWED);
//     // uint8_t temp[100]= "teslt";
//     // if (intelligentElectricBox->set_value_raw(temp,sizeof(temp)) != true)
//     // {
//     //     printf("intelligentElectricBox->set_value() failed\n");
//     //     return -1;
//     // }

//     // PUT resource 3201/0/5853
//     m2m_put_res = M2MInterfaceFactory::create_resource(m2m_obj_list, 3201, 0, 5853, M2MResourceInstance::INTEGER, M2MBase::GET_PUT_ALLOWED);
//     if (m2m_put_res->set_value(0) != true)
//     {
//         printf("m2m_led_res->set_value() failed\n");
//         return -1;
//     }
//     if (m2m_put_res->set_value_updated_function(put_update) != true)
//     {
//         printf("m2m_put_res->set_value_updated_function() failed\n");
//         return -1;
//     }

//     // POST resource 3201/0/5850
//     m2m_post_res = M2MInterfaceFactory::create_resource(m2m_obj_list, 3201, 0, 5850, M2MResourceInstance::INTEGER, M2MBase::POST_ALLOWED);
//     if (m2m_post_res->set_execute_function(execute_post) != true)
//     {
//         printf("m2m_post_res->set_execute_function() failed\n");
//         return -1;
//     }

//     // POST resource 5000/0/1 to trigger deregister.
//     m2m_deregister_res = M2MInterfaceFactory::create_resource(m2m_obj_list, 5000, 0, 1, M2MResourceInstance::INTEGER, M2MBase::POST_ALLOWED);

//     // Use delayed response
//     m2m_deregister_res->set_delayed_response(true);

//     if (m2m_deregister_res->set_execute_function(deregister) != true)
//     {
//         printf("m2m_post_res->set_execute_function() failed\n");
//         return -1;
//     }

//     printf("Register Pelion Device Management Client\n\n");
//     cloud_client = new MbedCloudClient(client_registered, client_unregistered, client_error, NULL, update_progress);
//     cloud_client->add_objects(m2m_obj_list);
//     cloud_client->setup(network); // cloud_client->setup(NULL); -- https://jira.arm.com/browse/IOTCLT-3114

//     //************initial ADE7953
//     printf("Lucas: start main\n");
//     ADE7953 test(PA_9, PA_10);
//     ADE7953 test2(PE_1, PE_0);
//     printf("initial %d\n", test.init());
//     printf("initial %d\n", test2.init());
//     //****************************
//     while (cloud_client_running)
//     {
//         int in_char = getchar();
//         if (in_char == 'i')
//         {
//             print_client_ids(); // When 'i' is pressed, print endpoint info
//             continue;
//         }
//         else if (in_char == 'r')
//         {
//             (void)fcc_storage_delete(); // When 'r' is pressed, erase storage and reboot the board.
//             printf("Storage erased, rebooting the device.\n\n");
//             wait(1);
//             NVIC_SystemReset();
//         }
//         else if (in_char == '1')
//         {
//             printf("\n==========cloud_client_running==========\n\n");
//             int temp = test.getInstVoltage();
//             printf("%-15s 0x218 = %x\n", "InstVoltage", temp);
//             m2m_get_res->set_value(temp);
//             printf("InstVoltage=  %" PRIu64 "\n\n", m2m_get_res->get_value_int());
//             printf("================send over================\n\n");
//             continue;
//         }
//         else if (in_char == '2')
//         {
//             int temp2 = test.getInstVoltage();
//             printf("\n==========cloud_client_running==========\n\n");
//             printf("%-15s 0x218 = %x\n", "InstVoltage", temp2);

//             uint8_t temp3[100] = "testttt";
//             printf("%d\n",sizeof(temp3));
//             wait(2);
//             intelligentElectricBox->set_value(temp2);
//             printf("InstVoltage=  %" PRIu64 "\n\n", intelligentElectricBox->get_value_int());
//             // if (intelligentElectricBox->set_value_raw(temp3, sizeof(temp3)) != true)
//             //     printf("intelligentElectricBox->set_value() failed\n");
//             printf("================send over================\n\n");
//             continue;
//         }
//         else if (dispatch_host_command && in_char != 0x03)
//         {
//             /* Intercept other host commands */
//             dispatch_host_command(in_char);
//             continue;
//         }
//         else if (in_char > 0 && in_char != 0x03)
//         {                   // Ctrl+C is 0x03 in Mbed OS and Linux returns negative number
//             button_press(); // Simulate button press
//             continue;
//         }
//         deregister_client();
//         break;
//     }
//     return 0;
// }

// #endif /* MBED_TEST_MODE */