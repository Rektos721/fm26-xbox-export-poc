using System.Reflection.Metadata;
using System.Reflection.PortableExecutable;

if (args.Length != 1)
{
    Console.Error.WriteLine("usage: fm26_meta_reader <assembly-path>");
    return 1;
}

using var stream = File.OpenRead(args[0]);
using var pe = new PEReader(stream);
if (!pe.HasMetadata)
{
    Console.Error.WriteLine("No metadata");
    return 2;
}

var reader = pe.GetMetadataReader();

Console.WriteLine("== Types ==");
foreach (var handle in reader.TypeDefinitions)
{
    var type = reader.GetTypeDefinition(handle);
    var ns = reader.GetString(type.Namespace);
    var name = reader.GetString(type.Name);
    if (name == "<Module>") continue;
    Console.WriteLine(string.IsNullOrEmpty(ns) ? name : $"{ns}.{name}");

    foreach (var methodHandle in type.GetMethods())
    {
        var method = reader.GetMethodDefinition(methodHandle);
        var methodName = reader.GetString(method.Name);
        Console.WriteLine($"  M {methodName}");
    }

    foreach (var fieldHandle in type.GetFields())
    {
        var field = reader.GetFieldDefinition(fieldHandle);
        var fieldName = reader.GetString(field.Name);
        Console.WriteLine($"  F {fieldName}");
    }

    foreach (var propHandle in type.GetProperties())
    {
        var prop = reader.GetPropertyDefinition(propHandle);
        var propName = reader.GetString(prop.Name);
        Console.WriteLine($"  P {propName}");
    }
}

return 0;
