using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml.Serialization;
using TaskMan.Models;

namespace TaskMan
{
    public partial class MainForm : Form
    {
        private List<ProcessInfo> processesInfo;
        private System.Threading.Timer timer;

        public MainForm()
        {
            InitializeComponent();
            Init();
        }

        private void Init()
        {
            timer = new System.Threading.Timer(CallBack, null, 0, 8000);
        }

        private void CallBack(object state)
        {
            LoadProcessesInfo();
        }

        private void LoadProcessesInfo()
        {
            //this.Invoke(new MethodInvoker(() => this.reportButton.Enabled = false)); // Программа вылетает при использовании этой конструкции. Во время дебага всё ОК.
            this.reportButton.Enabled = false;
            LoadProcesses();
            //this.Invoke(new MethodInvoker(() => this.reportButton.Enabled = true));
            this.reportButton.Enabled = true;
            this.Invoke(new MethodInvoker(LoadListView));
        }

        private void LoadProcesses()
        {
            processesInfo = new List<ProcessInfo>();
            var processes = Process.GetProcesses();
            foreach (var procs in processes)
            {
                ProcessInfo procsInfo = new ProcessInfo();
                procsInfo.Id = procs.Id;
                procsInfo.Name = procs.ProcessName;
                procsInfo.Threads = procs.Threads.Count;
                procsInfo.Handles = procs.HandleCount;

                // Закомментировать для увеличения скорости работы программы
                PerformanceCounter ram = new PerformanceCounter("Process", "Working Set", procs.ProcessName);
                procsInfo.RAM = ram.NextValue() / 1024;
                PerformanceCounter cpu = new PerformanceCounter("Process", "% Processor Time", procs.ProcessName);
                procsInfo.CPU = cpu.NextValue();
                //

                processesInfo.Add(procsInfo);
            }
        }

        private void LoadListView()
        {
            listView1.Items.Clear();
            for (int i = 0; i < processesInfo.Count(); i++)
            {
                var procs = processesInfo[i];

                listView1.Items.Add(new ListViewItem(new string[] {
                    procs.Id.ToString(),
                    procs.Name,
                    procs.Threads.ToString(),
                    procs.Handles.ToString(),
                    procs.RAM.ToString() + " KB",
                    procs.CPU.ToString() + " %"})
                {
                    Tag = procs
                }); 
            }
        }

        private void KillProcess(Process process)
        {
            try
            {
                process.Kill();
            }
            catch (Exception)
            {
                MessageBox.Show("Cannot kill process!", "Exeption", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void StartProcess(string fileName)
        {
            Process.Start(fileName);
        }

        private void KillToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (listView1.SelectedItems.Count > 0)
            {
                foreach (ListViewItem item in listView1.SelectedItems)
                {
                    ProcessInfo info = (ProcessInfo)item.Tag;
                    Process process = Process.GetProcessById(info.Id);
                    if (process != null)
                    {
                        KillProcess(process);
                    }
                }
                LoadProcessesInfo();
            }
        }

        private void Button2_Click(object sender, EventArgs e)
        {
            RunProcessForm form = new RunProcessForm();
            if (form.ShowDialog() == DialogResult.OK)
            {
                string str = form.FileName;
                if (string.IsNullOrWhiteSpace(str) == false)
                {
                    StartProcess(form.FileName);
                }
            }
        }

        private void Button1_Click_2(object sender, EventArgs e)
        {
            DialogResult result = this.saveFileDialog1.ShowDialog();
            if (result == DialogResult.OK)
            {
                string fName = this.saveFileDialog1.FileName;
                if (this.saveFileDialog1.FilterIndex == 1)
                {
                    SaveTxtReport(fName, processesInfo);
                }
                else if (this.saveFileDialog1.FilterIndex == 2)
                {
                    SaveXmlReport(fName, processesInfo);
                }

            }
        }

        private void SaveTxtReport(string fileName, List<ProcessInfo> processesInfo)
        {
            using (StreamWriter stream = File.CreateText(fileName))
            {
                stream.WriteLine($"Data: {DateTime.Now.ToString()}\nProcesses:");
                foreach (var procs in processesInfo)
                {
                    stream.WriteLine(procs.ToString());
                }
            }
        }

        private void SaveXmlReport(string fileName, List<ProcessInfo> processesInfo)
        {
            ProcessesReport report = new ProcessesReport();
            report.Date = DateTime.Now;
            report.ProcessesInfo = processesInfo;
            using (StreamWriter stream = File.CreateText(fileName))
            {
                XmlSerializer serializer = new XmlSerializer(typeof(ProcessesReport));
                serializer.Serialize(stream, report);
            }
        }
    }
}
