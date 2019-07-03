using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace TaskMan.Models
{
    [Serializable]
    public class ProcessesReport
    {
        public DateTime Date { get; set; }
        public List<ProcessInfo> ProcessesInfo { get; set; }
    }
}
