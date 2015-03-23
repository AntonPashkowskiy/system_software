using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DomainsDemo.PluginInterface
{
    public enum Complexity { Linear, Quadratic }

    [AttributeUsage( AttributeTargets.Class, AllowMultiple = false, Inherited = false )]
    public sealed class PluginAttribute : Attribute
    {
        private readonly bool _asynchOperation = false;
        private readonly string _name;
        private readonly Complexity _complecity;

        public PluginAttribute(string name) : this( name, Complexity.Linear ){}

        public PluginAttribute(string name, Complexity complecity)
        {
            _name = name;
            _complecity = complecity;
        }

        public PluginAttribute(string name, Complexity complecity, bool asynchOperation)
        {
            _name = name;
            _complecity = complecity;
            _asynchOperation = asynchOperation;
        }

        public PluginAttribute(string name, bool asynchOperation) : this(name, Complexity.Linear) 
        {
            _asynchOperation = asynchOperation;
        }

        public string Name { get { return _name; } }
        public bool AsynchOperation { get { return _asynchOperation; } }
    }
}
