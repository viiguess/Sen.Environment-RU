namespace Sen.Script.Executor.Methods.Data.Base64.Decode {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        destination?: string;
    }

    /**
     * Argument for batch method
     */

    export interface BatchArgument extends Sen.Script.Executor.Base {
        directory: string;
    }

    /**
     * Async support
     */

    export interface AsyncArgument<source extends string, destination extends string> extends Sen.Script.Executor.Base {
        worker: Array<[source, destination]>;
    }

    /**
     * Configuration file if needed
     */

    export interface Configuration extends Sen.Script.Executor.Configuration {}

    /**
     * ----------------------------------------------
     * JavaScript forward method, this method need
     * to be evaluated during script loading time
     * ----------------------------------------------
     */

    export function forward(): void {
        Sen.Script.Executor.push_as_module<
            Sen.Script.Executor.Methods.Data.Base64.Decode.Argument,
            Sen.Script.Executor.Methods.Data.Base64.Decode.BatchArgument,
            Sen.Script.Executor.Methods.Data.Base64.Decode.AsyncArgument<string, string>,
            Sen.Script.Executor.Methods.Data.Base64.Decode.Configuration
        >({
            id: `data.base64.decode`,
            configuration_file: Sen.Script.Home.query(`~/Executor/Configuration/data.base64.decode.json`),
            direct_forward(argument: Sen.Script.Executor.Methods.Data.Base64.Decode.Argument): void {
                Sen.Script.Executor.clock.start_safe();
                Sen.Script.Console.obtained(argument.source);
                Sen.Script.Executor.defined_or_default<Sen.Script.Executor.Methods.Data.Base64.Decode.Argument, string>(argument, `destination`, Sen.Kernel.Path.resolve(`${argument.source}.bin`));
                Sen.Script.Console.output(argument.destination!);
                Sen.Kernel.Encryption.Base64.decode_fs(argument.source, argument.destination!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.Data.Base64.Decode.BatchArgument): void {
                let count: bigint = 0n;
                Sen.Kernel.FileSystem.read_directory(argument.directory)
                    .filter((path: string) => Sen.Kernel.FileSystem.is_file(path))
                    .forEach((source: string) => {
                        this.direct_forward({ source });
                        count++;
                    });
                Sen.Script.Console.send(`Processed ${count} files`);
                return;
            },
            async_forward(argument: Sen.Script.Executor.Methods.Data.Base64.Decode.AsyncArgument<string, string>): void {
                Sen.Kernel.Encryption.Base64.decode_fs_as_multiple_threads<string, string>(argument.worker);
                return;
            },
            is_enabled: false,
            configuration: undefined!,
        });
        return;
    }
}

Sen.Script.Executor.Methods.Data.Base64.Decode.forward();