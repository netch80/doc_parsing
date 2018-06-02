class ScalarSV extends SemanticValue {

    double value;

    ScalarSV(double nvalue) {
        value = nvalue;
    }

    @Override
    double getRValue(ExecutionContext ec_) {
        return value;
    }
}
