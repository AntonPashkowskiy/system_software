using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DomainsDemo.PluginInterface;

namespace StatisticPlugin
{
	[Serializable]
    [Plugin( "Mean" )]
    public class Mean : PluginBase
    {
        public override double Operation(double[] input)
        {
            return input.Average();
        }
    }
}
