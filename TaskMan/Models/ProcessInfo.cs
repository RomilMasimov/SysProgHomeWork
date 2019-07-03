using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace TaskMan.Models
{
    [Serializable]
    [XmlType("Process")]
    public class ProcessInfo
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public int Threads { get; set; }
        public int Handles { get; set; }
        public double CPU { get; set; }
        public double RAM { get; set; }

        public override string ToString()
        {
            return $"{Id.ToString()} | {Name} | {Threads.ToString()} | {Handles.ToString()} | {RAM.ToString()} KB | {CPU.ToString()} %";
        }
    }
}
