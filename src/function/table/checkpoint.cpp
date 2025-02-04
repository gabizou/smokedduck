#include "duckdb/function/table/range.hpp"
#include "duckdb/main/client_context.hpp"
#include "duckdb/storage/storage_manager.hpp"
#include "duckdb/transaction/transaction_manager.hpp"

namespace duckdb {

static unique_ptr<FunctionData> CheckpointBind(ClientContext &context, vector<Value> &inputs,
                                               unordered_map<string, Value> &named_parameters,
                                               vector<LogicalType> &input_table_types,
                                               vector<string> &input_table_names, vector<LogicalType> &return_types,
                                               vector<string> &names) {
	return_types.push_back(LogicalType::BOOLEAN);
	names.emplace_back("Success");
	return nullptr;
}

template <bool FORCE>
#ifdef LINEAGE
static void TemplatedCheckpointFunction(ExecutionContext &context, const FunctionData *bind_data_p,
                                        FunctionOperatorData *operator_state, DataChunk *input, DataChunk &output) {
	auto &transaction_manager = TransactionManager::Get(context.client);
	transaction_manager.Checkpoint(context.client, FORCE);
}

#else
static void TemplatedCheckpointFunction(ClientContext &context, const FunctionData *bind_data_p,
                                        FunctionOperatorData *operator_state, DataChunk *input, DataChunk &output) {
	auto &transaction_manager = TransactionManager::Get(context);
	transaction_manager.Checkpoint(context, FORCE);
}
#endif

void CheckpointFunction::RegisterFunction(BuiltinFunctions &set) {
	TableFunction checkpoint("checkpoint", {}, TemplatedCheckpointFunction<false>, CheckpointBind);
	set.AddFunction(checkpoint);
	TableFunction force_checkpoint("force_checkpoint", {}, TemplatedCheckpointFunction<true>, CheckpointBind);
	set.AddFunction(force_checkpoint);
}

} // namespace duckdb
