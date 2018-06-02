class IdentifierSV extends SemanticValue {
    String varname;

    IdentifierSV(String nvarname) {
        varname = nvarname;
    }

    double getRValue(ExecutionContext econtext) {
        return econtext.scalars.getOrDefault(varname, Double.NaN);
    }

    void assignValue(double new_value, ExecutionContext econtext) {
        econtext.scalars.put(varname, new_value);
    }
}
