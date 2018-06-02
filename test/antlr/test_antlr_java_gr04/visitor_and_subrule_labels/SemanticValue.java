class SemanticValue {
    double getRValue(ExecutionContext econtext) {
        throw new DCExecutionError("Not implemented: getRValue");
    }

    void assignValue(double new_value, ExecutionContext econtext) {
        throw new DCExecutionError("Not implemented: assignValue");
    }
}
