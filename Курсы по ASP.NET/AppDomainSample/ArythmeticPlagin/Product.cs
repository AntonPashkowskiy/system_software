using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DomainsDemo.PluginInterface;

namespace ArythmeticPlugin
{
	
    [Plugin("Product")]
	[Serializable]
    public class Product : PluginBase
    {
        public override double Operation(double[] input)
        {
            return input.Aggregate(1.0, (acc, x) => acc *= x);
        }
    }
}
