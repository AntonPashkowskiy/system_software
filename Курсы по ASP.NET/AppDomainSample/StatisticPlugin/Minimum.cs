using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DomainsDemo.PluginInterface;

namespace StatisticPlugin
{
	[Serializable]
    [Plugin("Minimum")]
    public class Minimum : PluginBase
    {
        public override double Operation(double[] input)
        {
            return input.Min();
        }
    }
}
