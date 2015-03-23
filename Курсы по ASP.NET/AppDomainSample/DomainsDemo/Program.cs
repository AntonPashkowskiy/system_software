using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using DomainsDemo.PluginInterface;

namespace DomainsDemo
{
	struct Result
	{
		public string Class { get; set; }
		public double Result { get; set; }
	}

    class Program
    {
		// поскольку часть заданий будет выполнятся ассинхронно, 
		// то есть момент когда ассинхронная задача выполнится не определён
		// для хранения результатов будем использовать список (отработавший класс - результат)
			
		private static List<Result> _results = new List<Result>();
		private static int _asynchOperationCount = 0;
		
		public static void Main( string[] args )
		{
			// инициализация очереди действий с данными
			Queue<Action> actions = new Queue<Action>();
			actions.Enqueue( new Action( "mean", new double[7] { 2, 4, 5, 33, 44, 6, 7 } ) );
			actions.Enqueue( new Action( "maximum", new double[ 6 ] { 3, 66, 44, 33, 11, 90 } ) );
			actions.Enqueue( new Action( "sum", new double[ 5 ] { 321, 66, 44, 33, 112 } ) );
			actions.Enqueue( new Action( "variance", new double[ 8 ] { 3, 66, 44, 33, 11, 90, 880, 33 } ) );
			actions.Enqueue( new Action( "minimum", new double[ 8 ] { 3, 66, 44, 33, 11, 90, 880, 33 } ) );

			try
			{
				// получение правил загрузки плагинов для определённых в очереди действий						
				List<ActionPlugin> actionPlugins = ActionPluginLoader.Load( "ActionPlugins.xml" );

				if ( actionPlugins == null )
				{
					Console.WriteLine( "Правил загрузки плагинов не найдено." );
					Console.ReadKey();
					return;
				}
				
				ActionPlugin actionPlugin = null;
				Action action = null;
				// создание домена для плагинов
				var pluginDomain = AppDomain.CreateDomain( "Domain.Plugins" );
				AppDomain.CurrentDomain.ReflectionOnlyAssemblyResolve += ReflectionOnlyLoad; 

				while ( actions.Count != 0 )
				{
					action = actions.Dequeue();
					// ищем операцию среди плагинов
					foreach ( var plugin in actionPlugins )
					{
						if( plugin.Operation == action.Operation )
						{
							actionPlugin = plugin;
							break;
						}
					}

					// не нашли - кидаем исключение
					if( actionPlugin == null )
					{
						throw new Exception( "Нет правила для данной операции." );
					}
					
					
					var asmRef = Assembly.ReflectionOnlyLoadFrom( actionPlugin.Assembly );
					// получаем все типы из сборки
					var types = asmRef.GetExportedTypes();

					foreach ( var type in types )
					{
						// получаем интерфейс, атрибут и базовый тип типа
						var typeInterface = type.GetInterface( "DomainsDemo.PluginInterface.IPlugin" );
						var attributesData = type.GetCustomAttributesData();
						var baseType = type.BaseType;
						PluginAttribute attribute = GetAttribute( attributesData );

						if ( attribute == null ) continue;

						if( baseType == null || typeInterface == null || attribute.Name != actionPlugin.Class  )
						{
							continue;
						}

						// строим экземпляр класса, приводя его к интерфейсу IPlugin
						IPlugin plugin = ( IPlugin )pluginDomain.CreateInstanceFromAndUnwrap( asmRef.Location, type.FullName );
						double result = 0;
						
						// вычисляем результат операции
						if( attribute.AsynchOperation )
						{
							ThreadPool.QueueUserWorkItem( ExecuteOperation, 
														  new AsynchOperationArguments( actionPlugin.Class, plugin, action.Data ) 
														);
							lock( _results )
							{
								_asynchOperationCount ++;
							}
						}
						else
						{
							result = plugin.Operation( action.Data );
							
							lock ( _results )
							{
								_results.Add( new Result { Class = actionPlugin.Class, Result = result } );
							}	
						}
						
						break;
					}
				}

				// дожидаемся выполнения всех ассинхронных операций
				while ( _asynchOperationCount != 0 );
				
				// выводим результаты операций
				foreach ( var result in _results )
				{
					Console.WriteLine( "Class: {0}. \nResult: {1:0.00}.\n", result.Class, result.Result );
				}
				
				// выводим загруженные во время выполнения сборки
				var assemblies = pluginDomain.GetAssemblies();
				Console.WriteLine( "\n{0} assemblyes was loaded.\n", assemblies.Count() );

				foreach ( var asm in assemblies )
				{
					Console.WriteLine( asm.FullName );
				}

				AppDomain.Unload( pluginDomain );
			}
			catch ( Exception ex )
			{
				Console.WriteLine( ex.Message );
			}

			Console.ReadKey();
		}

		// функция для получения нормального атрибута из кучи данных
		// нужна из-за невозможности создать объект атрибута загрузив сборку только для отражения 
		private static PluginAttribute GetAttribute(IList<CustomAttributeData> attributesData)
		{
			foreach ( var attr in attributesData )
			{
				if( attr.AttributeType.Name == typeof( PluginAttribute ).Name )
				{
					string name = null;
					bool asynchOperation = false;
					var constructorArguments = attr.ConstructorArguments;
					
					foreach ( var arg in constructorArguments )
					{
						if( arg.ArgumentType == typeof( string ) )
						{
							name = arg.Value.ToString();
						}

						if( arg.ArgumentType == typeof( bool ) )
						{
							asynchOperation = ( bool )arg.Value;
						}
					}

					if ( name != null )
					{
						return new PluginAttribute( name, asynchOperation ); 
					}
					else
					{
						return null;
					}
				}
			}

			return null;
		}

		private static void ExecuteOperation( object arguments )
		{
			AsynchOperationArguments args = arguments as AsynchOperationArguments;

			if( args == null || args.Instance == null || args.Data == null )
			{
				return;
			}
			else
			{
				double result = args.Instance.Operation( args.Data );
 
				lock( _results )
				{
					_results.Add( new Result { Class = args.Class, Result = result } );
					_asynchOperationCount --;
				}
			}
		}

		private static Assembly ReflectionOnlyLoad( object sender, ResolveEventArgs args )
		{
			return Assembly.ReflectionOnlyLoad( args.Name );
		}
    }
}
