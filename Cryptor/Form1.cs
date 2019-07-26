using Cryptor.Encoders;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Cryptor
{
    public partial class Form1 : Form
    {
        private CancellationTokenSource cts;

        public Form1()
        {
            InitializeComponent();
        }

        private void OpenFileButton_Click(object sender, EventArgs e)
        {
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                filePathTextBox.Text = openFileDialog.FileName;
            }
        }

        private void CancelButton_Click(object sender, EventArgs e)
        {
            cts?.Cancel();
            this.progressBar.Value = this.progressBar.Maximum;
            this.startButton.Enabled = true;
            this.openFileButton.Enabled = true;
            this.passwordTextBox.Enabled = true;
        }

        private void TextBox_TextChanged(object sender, EventArgs e)
        {
            startButton.Enabled = (filePathTextBox.Text.Length > 0 && 
                                   File.Exists(filePathTextBox.Text) &&
                                   passwordTextBox.Text.Length > 0);
        }

        private void StartButton_Click(object sender, EventArgs e)
        {
            this.progressBar.Maximum = 3;
            this.progressBar.Value = 0;
            this.startButton.Enabled = false;
            this.openFileButton.Enabled = false;
            this.passwordTextBox.Enabled = false;

            var context = SynchronizationContext.Current;

            cts = new CancellationTokenSource();
            var token = cts.Token;

            Task.Run(() =>
            {
                byte[] result;

                token.ThrowIfCancellationRequested();

                using (FileStream SourceStream = File.Open(filePathTextBox.Text, FileMode.Open))
                {
                    result = new byte[SourceStream.Length];
                    SourceStream.Read(result, 0, (int)SourceStream.Length);
                }
                context.Post(_ => this.progressBar.Value++, null);

                Thread.Sleep(3000);
                token.ThrowIfCancellationRequested();

                result = MyEncoder.Encode(result, passwordTextBox.Text);
                context.Post(_ => this.progressBar.Value++, null);

                using (FileStream SourceStream = File.OpenWrite(filePathTextBox.Text))
                {
                    SourceStream.Write(result, 0, result.Length);
                }
                context.Post(_ => this.progressBar.Value++, null);
            }, token).ContinueWith(_ =>
            {
                this.startButton.Enabled = true;
                this.openFileButton.Enabled = true;
                this.passwordTextBox.Enabled = true;
                MessageBox.Show("Done");
            });

        }
    }
}
