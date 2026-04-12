using Mono.Cecil;
using Mono.Cecil.Cil;

if (args.Length < 2)
{
    Console.Error.WriteLine("usage: fm26_il_dump <assembly-path> <type-name>");
    return 1;
}

var asm = AssemblyDefinition.ReadAssembly(args[0]);
var type = asm.MainModule.Types.FirstOrDefault(t => t.FullName == args[1]);
if (type is null)
{
    Console.Error.WriteLine("type not found");
    return 2;
}

foreach (var method in type.Methods)
{
    Console.WriteLine($"=== {method.FullName} ===");
    if (!method.HasBody)
    {
        Console.WriteLine("<no body>");
        Console.WriteLine();
        continue;
    }

    foreach (var ins in method.Body.Instructions)
    {
        string operand = ins.Operand switch
        {
            null => "",
            MethodReference m => m.FullName,
            FieldReference f => f.FullName,
            TypeReference t => t.FullName,
            ParameterDefinition p => p.Name,
            VariableDefinition v => v.VariableType.FullName + " V_" + v.Index,
            Instruction target => "IL_" + target.Offset.ToString("X4"),
            Instruction[] targets => string.Join(", ", targets.Select(t => "IL_" + t.Offset.ToString("X4"))),
            _ => ins.Operand.ToString() ?? ""
        };
        Console.WriteLine($"IL_{ins.Offset:X4}: {ins.OpCode} {operand}".TrimEnd());
    }

    Console.WriteLine();
}

return 0;
