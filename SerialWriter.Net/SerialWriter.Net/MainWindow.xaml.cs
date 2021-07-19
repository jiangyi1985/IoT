using System;
using System.Diagnostics;
using System.IO.Ports;
using System.Linq;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using LibreHardwareMonitor.Hardware;

namespace SerialWriter.Net
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private Computer _c;
        private Timer _timer;
        private SerialPort port = new SerialPort();
        public MainWindow()
        {
            InitializeComponent();

            LoadForm();
        }

        private void LoadForm()
        {
            LoadPortList();

            _c = new Computer { IsCpuEnabled = true, IsGpuEnabled = true, };
            _c.Open();
            foreach (var hardware in _c.Hardware)
            {
                Trace.WriteLine($"{hardware.HardwareType}  {hardware.Name}  {hardware.Identifier}");
                Trace.WriteLine($"");
                foreach (var sensor in hardware.Sensors)
                {
                    Trace.WriteLine($"{sensor.SensorType}  {sensor.Name}  {sensor.Value}  {sensor.Max}  {sensor.Min}");
                }
                Trace.WriteLine($"");
                Trace.WriteLine($"");
            }

            _timer = new Timer(Tick, null, TimeSpan.Zero, TimeSpan.FromMilliseconds(500));
        }

        private void LoadPortList()
        {
            var portNames = SerialPort.GetPortNames();
            foreach (var portName in portNames)
            {
                cbSerialPorts.Items.Add(portName);
            }

            if (portNames.Length > 0)
            {
                cbSerialPorts.SelectedIndex = 0;
            }
        }

        private void Tick(object state)
        {
            //_c=new Computer { IsCpuEnabled = true, IsGpuEnabled = true, };
            //_c.Open();
            var cpu = _c.Hardware.FirstOrDefault(o => o.HardwareType == HardwareType.Cpu);
            cpu.Update();
            var cpuTotal = cpu.Sensors.FirstOrDefault(o => o.Name == "CPU Total");

            Trace.WriteLine($"{cpuTotal.SensorType}  {cpuTotal.Name}  {cpuTotal.Value}  {cpuTotal.Max}  {cpuTotal.Min}");

            var cpuTotalAvg = cpuTotal.Value;

            var str = cpuTotalAvg.Value.ToString("0.00") + "\n";

            if (port.IsOpen)
            {
                Trace.WriteLine("sending " + str);
                port.Write(str);
            }
        }

        private void cbSerialPorts_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            port.PortName = ((ComboBox)e.Source).SelectedValue.ToString();
            if (!port.IsOpen)
            {
                port.BaudRate = 115200;
                port.Open();
            }
        }
    }
}
