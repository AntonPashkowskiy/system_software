using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DomainsDemo.PluginInterface;

namespace StatisticPlugin
{
	[Serializable]
    [Plugin("Median", Complexity.Quadratic, true)]
    public class Median : PluginBase
    {
        public override double Operation(double[] input)
        {
            Array.Sort(input);

            if (input.Length % 2 == 0)
            {
                return (input[input.Length / 2] + input[input.Length / 2 - 1]) / 2; 
            }

            return input[ input.Length / 2 ]; 
        }
    }
}
