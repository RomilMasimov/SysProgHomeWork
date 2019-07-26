using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Copy
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private async Task Copy()
        {
            var from = this.fromPathTextBox.Text;
            var to = this.toPathTextBox.Text;

            if (!File.Exists(from))
            {
                MessageBox.Show("File not found!", "Error", MessageBoxButtons.OK);
                return;
            }

            // Обнуляю файл
            if (File.Exists(to))
            {
                File.WriteAllText(to, "");
            }

            using (FileStream fsr = File.OpenRead(from))
            {
                var it = (fsr.Length / 4096) + 1;

                this.button3.Enabled = false;
                this.progressBar1.Value = 0;
                this.progressBar1.Maximum = (int)it;
                this.progressBar1.Step = 1;
                
                for (int i = 0; i < it; i++)
                {
                    var bytes = new byte[4096];
                    await fsr.ReadAsync(bytes, 0, 4096);

                    using (FileStream fsw = new FileStream(to, FileMode.Append))
                    {
                        var count = bytes.Count();
                        await fsw.WriteAsync(bytes, 0, count);
                    }
                    this.progressBar1.Value += 1;
                }
            }
            this.button3.Enabled = true;
        }

        private void Button1_Click(object sender, EventArgs e)
        {
            var r = this.openFileDialog1.ShowDialog();
            if (r == DialogResult.OK)
            {
                this.fromPathTextBox.Text = this.openFileDialog1.FileName;
            }
        }

        private void Button2_Click(object sender, EventArgs e)
        {
            var r = this.saveFileDialog1.ShowDialog();
            if (r == DialogResult.OK)
            {
                this.toPathTextBox.Text = this.saveFileDialog1.FileName;
            }
        }

        private void Button3_Click(object sender, EventArgs e)
        {
            Copy();
        }

        private async void ToPathTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyData == Keys.Enter)
            {
                Copy();
            }
        }
    }
}
