using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Android;
using Android.App;
using Android.Content;
using Android.Content.PM;
using Android.Locations;
using Android.OS;
using Android.Runtime;
using Android.Support.V4.Content;
using Android.Views;
using Android.Widget;
using nexus.core;
using nexus.protocols.ble;
using nexus.protocols.ble.gatt;
using nexus.protocols.ble.scan;

namespace TPMS_App
{
    class BLE
    {
        // Fields
        private Activity g_activity = null;
        private Context g_context = null;
        private Guid g_BLETargetService = new Guid();
        private Guid g_BLETargetCharacteristic = new Guid();
        private IBluetoothLowEnergyAdapter g_BLEAdapterObj = null;
        private BLECommandsParser g_BLECommandsParserObj = null;
        private Dictionary<string, IBlePeripheral> g_BLEScanDevices = new Dictionary<string, IBlePeripheral>();

        private Action<EnabledDisabledState> g_BLEAdapterObj_stateChangeObserver = null;
        private Action<ConnectionState> g_BLEAdapterObj_connectionStateObserver = null;
        private Action<Guid, Byte[]> g_BLEAdapterObj_notificationServiceObserver = null;
        private Action<IBlePeripheral> g_BLEAdapterObj_scanResponsesObserver = null;
        private OnLocationAdapterStateChange g_LocationAdapterStateChangeObserver = null;
        private IBleGattServerConnection g_BLEAdapterObj_connectionGattServer = null;

        // Private methods

        /* GPS Adapter callbacks */
        private void OnLocationMsgButtonClick(object sender, DialogClickEventArgs e)
        {
            // Show location settings when the user acknowledges the alert dialog
            Intent intent = new Intent(Android.Provider.Settings.ActionLocationSourceSettings);
            g_activity.StartActivity(intent);
        }

        /* BLE Adapter Callbacks */
        private void OnBLEAdapter_StateChange(EnabledDisabledState state)
        {
            if ((EnabledDisabledState.Enabled != state) && (EnabledDisabledState.Enabling != state))
                Library.CloseApplication(g_activity);
        }
        private void OnBLEAdapter_ScanResponses(IBlePeripheral peripheral)
        {
            string deviceName = peripheral.Advertisement.DeviceName;

            if (null != deviceName)
                g_BLEScanDevices.TryAdd(deviceName, peripheral);
        }
        private void OnBLEConnection_StateChange(ConnectionState connectionState)
        {
            if ((ConnectionState.Connected != connectionState) && (ConnectionState.Connecting != connectionState))
            {
                if (null != g_BLEAdapterObj_connectionGattServer)
                    g_BLEAdapterObj_connectionGattServer.Disconnect();
            }
        }
        private void OnBLEConnection_ServiceNotification(Guid characteristic, byte[] data)
        {
            string characteristicString = characteristic.ToString();

            if (true == string.Equals(characteristicString, Library.BLE_DEVICE_CHARACTERISTIC_UUID_TARGET, StringComparison.OrdinalIgnoreCase))
            {
                if(null != g_BLECommandsParserObj)
                    g_BLECommandsParserObj.ParseIncomingData(data);
            }
        }

        /* Internal */
        private async Task<bool> SetupBLEConnection(BlePeripheralConnectionRequest connection)
        {
            bool targetFound = false;
            bool setupDone = false;

            /* Look for Service UUID and Characteristic UUID on device Target */
            try
            {
                IEnumerable<Guid> services = await connection.GattServer.ListAllServices();

                foreach (Guid service in services)
                {
                    string serviceString = service.ToString();

                    if (true == string.Equals(serviceString, Library.BLE_DEVICE_SERVICE_UUID_TARGET, StringComparison.OrdinalIgnoreCase))
                    {
                        g_BLETargetService = service;
                        IEnumerable<Guid> characteristics = await connection.GattServer.ListServiceCharacteristics(service);

                        foreach (Guid characteristic in characteristics)
                        {
                            g_BLETargetCharacteristic = characteristic;
                            string characteristicString = characteristic.ToString();

                            if (true == string.Equals(characteristicString, Library.BLE_DEVICE_CHARACTERISTIC_UUID_TARGET, StringComparison.OrdinalIgnoreCase))
                                targetFound = true;
                        }
                    }
                }
            }
            catch (Exception)
            {

            }

            if (true == targetFound)
            {
                // Subcribe to notifications of Target characteristic
                try
                {
                    g_BLEAdapterObj_notificationServiceObserver = OnBLEConnection_ServiceNotification;
                    connection.GattServer.NotifyCharacteristicValue(g_BLETargetService, g_BLETargetCharacteristic, g_BLEAdapterObj_notificationServiceObserver);
                    setupDone = true;
                }
                catch (GattException)
                {

                }

                if (true == setupDone)
                {
                    // Suscribe to receive BLE Connection state changes
                    g_BLEAdapterObj_connectionStateObserver = OnBLEConnection_StateChange;
                    connection.GattServer.Subscribe(Observer.Create<ConnectionState>(g_BLEAdapterObj_connectionStateObserver, null, null));

                    // Get Gatt Server handler
                    g_BLEAdapterObj_connectionGattServer = connection.GattServer;
                }
            }

            return setupDone;
        }

        // Public methods
        public async Task<bool> Initialize(Activity activity, Context context, BLECommandsParser.cmdTableDel_t action)
        {
            bool ret = false;

            try
            {
                g_activity = activity;
                g_context = context;

                // Initialize bluetooth adapter
                BluetoothLowEnergyAdapter.Init(activity);

                // Obtain bluetooth adapter handler
                g_BLEAdapterObj = BluetoothLowEnergyAdapter.ObtainDefaultAdapter(context);

                // Enable BLE adapter in case it is disabled. Close application if user denie
                if (g_BLEAdapterObj.AdapterCanBeEnabled && g_BLEAdapterObj.CurrentState.IsDisabledOrDisabling())
                {
                    await g_BLEAdapterObj.EnableAdapter();

                    if (true == g_BLEAdapterObj.CurrentState.IsDisabledOrDisabling())
                        Library.CloseApplication(activity);
                }

                // Suscribe to receive BLE adapter state changes
                g_BLEAdapterObj_stateChangeObserver = OnBLEAdapter_StateChange;
                g_BLEAdapterObj.CurrentState.Subscribe(Observer.Create<EnabledDisabledState>(g_BLEAdapterObj_stateChangeObserver, null, null));

                // Suscribe to receive Location adapter state changes
                g_LocationAdapterStateChangeObserver = new OnLocationAdapterStateChange();
                activity.RegisterReceiver(g_LocationAdapterStateChangeObserver, new IntentFilter(LocationManager.ProvidersChangedAction));

                // Enable location service permissions. Needed to make BLE device scan
                Permission permissionCheck = ContextCompat.CheckSelfPermission(activity, Manifest.Permission.AccessFineLocation);

                if (permissionCheck != Permission.Granted)
                {
                    if (true == Android.Support.V4.App.ActivityCompat.ShouldShowRequestPermissionRationale(activity, Manifest.Permission.AccessFineLocation))
                    {
                        Toast.MakeText(activity, "The permission to get BLE location data is required", ToastLength.Short).Show();
                    }
                    else
                    {
                        activity.RequestPermissions(new String[] { Manifest.Permission.AccessCoarseLocation, Manifest.Permission.AccessCoarseLocation }, 1);
                    }
                }
                else
                {
                    Toast.MakeText(activity, "Location permissions already granted", ToastLength.Short).Show();
                }

                // Get Location Manager and check for GPS & Network location services. Ask user to enable them if needed
                LocationManager lm = (LocationManager)context.GetSystemService(Context.LocationService);

                if (!lm.IsProviderEnabled(LocationManager.GpsProvider) || !lm.IsProviderEnabled(LocationManager.NetworkProvider))
                {
                    EventHandler<DialogClickEventArgs> ButtonClickCb = OnLocationMsgButtonClick;

                    // Build the alert dialog
                    Android.Support.V7.App.AlertDialog.Builder builder = new Android.Support.V7.App.AlertDialog.Builder(activity);
                    builder.SetTitle("Location Services Not Active");
                    builder.SetMessage("Please enable Location Services and GPS");
                    builder.SetPositiveButton("OK", ButtonClickCb);

                    Dialog alertDialog = builder.Create();
                    alertDialog.SetCanceledOnTouchOutside(false);
                    alertDialog.Show();
                }

                // Initialize BLE Commands Parser
                g_BLECommandsParserObj = new BLECommandsParser();
                Library.cmdTable += action;

                ret = true;
            }
            catch (Exception)
            {

            }

            return ret;
        }
        public async void BLE_Scan()
        {
            /* To be inserted in a fragment */
            LocationManager lm = (LocationManager)g_context.GetSystemService(Context.LocationService);

            if (lm.IsProviderEnabled(LocationManager.GpsProvider) && lm.IsProviderEnabled(LocationManager.NetworkProvider))
            {
                g_BLEAdapterObj_scanResponsesObserver = OnBLEAdapter_ScanResponses;
                await g_BLEAdapterObj.ScanForBroadcasts(Observer.Create<IBlePeripheral>(g_BLEAdapterObj_scanResponsesObserver), TimeSpan.FromSeconds(10));
            }
            else
            {
                Library.CloseApplication(g_activity);
            }
        }
        public async Task<bool> BLE_Connect(IBlePeripheral device)
        {
            // Get BLE device
            if ((null != g_BLEAdapterObj) && (null != device))
            {
                // Try connect to ble device
                BlePeripheralConnectionRequest connection = await g_BLEAdapterObj.ConnectToDevice(device, TimeSpan.FromSeconds(15));

                if (connection.IsSuccessful())
                {
                    if (true == await SetupBLEConnection(connection))
                    {
                        return true;
                    }
                    else
                    {
                        await connection.GattServer.Disconnect();
                    }
                }
            }

            return false;
        }
        public async Task<bool> BLE_ScanAndConnect(string deviceName)
        {
            // Get BLE device
            if (null != deviceName)
            {
                // Try connect to ble device
                ScanFilter filter = new ScanFilter().SetAdvertisedDeviceName(deviceName);

                BlePeripheralConnectionRequest connection = await g_BLEAdapterObj.FindAndConnectToDevice(filter, TimeSpan.FromSeconds(15));

                if (connection.IsSuccessful())
                {
                    if (true == await SetupBLEConnection(connection))
                    {
                        return true;
                    }
                    else
                    {
                        await connection.GattServer.Disconnect();
                    }
                }
            }

            return false;
        }
        public async Task<byte[]> BLE_ReadTargetCharacteristic()
        {
            try
            {
                if ((null != g_BLEAdapterObj_connectionGattServer) && (ConnectionState.Connected == g_BLEAdapterObj_connectionGattServer.State))
                {
                    return await g_BLEAdapterObj_connectionGattServer.ReadCharacteristicValue(g_BLETargetService, g_BLETargetCharacteristic);
                }
            }
            catch (GattException)
            {

            }

            return null;
        }
        public async Task<byte[]> BLE_WriteTargetCharacteristic(byte[] data)
        {
            try
            {
                if ((null != g_BLEAdapterObj_connectionGattServer) && (ConnectionState.Connected == g_BLEAdapterObj_connectionGattServer.State))
                {
                    return await g_BLEAdapterObj_connectionGattServer.WriteCharacteristicValue(g_BLETargetService, g_BLETargetCharacteristic, data);
                }
            }
            catch (GattException)
            {

            }

            return null;
        }
    }
}