namespace Sen.Script.Executor {
    // The base need to be an object that is declared later

    export type Base = {};

    // Base Configuration need to be inherited

    export interface Configuration {}

    // Method Executor should implement direct forward, batch forward and async forward

    export interface MethodExecutor<
        Argument extends Sen.Script.Executor.Base,
        BatchArgument extends Sen.Script.Executor.Base,
        AsyncArgument extends Sen.Script.Executor.Base,
        Configuration extends Sen.Script.Executor.Configuration,
    > {
        id: string;
        configuration_file: string;
        direct_forward: (argument: Argument) => void;
        batch_forward: (argument: BatchArgument) => void;
        async_forward?: (argument: AsyncArgument) => void;
        is_enabled: boolean;
        configuration: Configuration;
    }

    /**
     * Forwarder typical type
     */

    export enum Forward {
        DIRECT,
        BATCH,
        ASYNC,
    }

    /**
     * Clock need to be initialized during the runtime.
     * Clock will calculate everything
     */

    export const clock: Sen.Script.Clock = new Clock();

    /**
     *
     * All methods are assigned here as key | value
     * Key: must be the id of the typical module
     * Value: the worker
     *
     */

    const methods: Map<string, Sen.Script.Executor.MethodExecutor<Sen.Script.Executor.Base, Sen.Script.Executor.Base, Sen.Script.Executor.Base, Sen.Script.Executor.Configuration>> = new Map();

    /**
     * ----------------------------------------------------------
     * JavaScript Implementation of Executor
     * @param worker - Here, we assign worker with the typical
     * object that has been declared
     * @returns - and so, the typical method will be assign
     * if the key is not found
     * ----------------------------------------------------------
     */

    export function push_as_module<
        Argument extends Sen.Script.Executor.Base,
        BatchArgument extends Sen.Script.Executor.Base,
        AsyncArgument extends Sen.Script.Executor.Base,
        Configuration extends Sen.Script.Executor.Configuration,
    >(worker: MethodExecutor<Argument, BatchArgument, AsyncArgument, Configuration>): void {
        const primary_id: string = worker.id!;
        delete (worker as any).id;
        if (methods.get(primary_id) !== undefined) {
            throw new Error(`${primary_id} is already existed`);
        }
        methods.set(primary_id, worker as MethodExecutor<Base, Base, Base, Configuration>);
        return;
    }

    /**
     * ----------------------------------------------------------
     * JavaScript Executor Implement
     * @param argument - Argument to query
     * @param key - Key
     * @param defined_value - If not, this val will assign to it
     * @returns
     * ----------------------------------------------------------
     */

    export function defined_or_default<Argument extends Sen.Script.Executor.Base, T>(argument: Argument, key: string, defined_value: T): void {
        if ((argument as any & Argument)[key] === undefined) {
            (argument as any & Argument)[key] = defined_value;
        }
        return;
    }

    /**
     * ----------------------------------------------------------
     * JavaScript Implementation of Runner
     * @param id - Here, we call the id. If the id is assigned
     * as a method, the method will be called instantly
     * @param argument - Provide arguments
     * @param forward_type - Forwarder type
     * @returns - Launch if the method found.
     * If an error is thrown, it mean the id is not assigned
     * ----------------------------------------------------------
     */

    export function run_as_module<Argument extends Sen.Script.Executor.Base>(id: string, argument: Argument, forward_type: Sen.Script.Executor.Forward): void {
        const worker: Sen.Script.Executor.MethodExecutor<Sen.Script.Executor.Base, Sen.Script.Executor.Base, Sen.Script.Executor.Base, Sen.Script.Executor.Configuration> | undefined = methods.get(id);
        if (worker === undefined) {
            throw new Error(`Method ${id} not found`);
        }
        worker.configuration = Sen.Kernel.JSON.deserialize_fs<Configuration>(worker.configuration_file);
        Sen.Script.Console.send(Sen.Script.Setting.Language.format(Sen.Script.Setting.Language.get(`method_loaded`), Sen.Script.Setting.Language.get(id)!), Sen.Script.Definition.Console.Color.GREEN);
        switch (forward_type) {
            case Sen.Script.Executor.Forward.ASYNC: {
                if (worker.async_forward === undefined) {
                    throw new Error(Setting.Language.format(Setting.Language.get(`method_does_not_support_async_implementation`), id));
                }
                worker.async_forward(argument);
                break;
            }
            case Sen.Script.Executor.Forward.BATCH: {
                worker.batch_forward(argument);
                break;
            }
            case Sen.Script.Executor.Forward.DIRECT: {
                worker.direct_forward(argument);
                break;
            }
            default: {
                throw new Error(Sen.Script.Setting.Language.format(Setting.Language.get(`method_does_not_support_async_implementation`), forward_type));
            }
        }
        Sen.Script.Executor.clock.stop_safe();
        Sen.Script.Console.send(`${Sen.Script.Setting.Language.get(`execution_time`)}: ${Sen.Script.Executor.clock.duration.toFixed(3)}s`, Sen.Script.Definition.Console.Color.GREEN);
        return;
    }
}