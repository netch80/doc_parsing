import java.util.Map;

class IndexedReferenceSV extends SemanticValue {

    IdentifierSV base;
    SemanticValue index;

    IndexedReferenceSV(IdentifierSV nbase, SemanticValue nindex) {
        base = nbase;
        index = nindex;
    }

    @Override
    double getRValue(ExecutionContext econtext) {
        Map<Double, Double> themap = econtext.maps.get(base.varname);
        if (themap == null) {
            throw new DCExecutionError("Map not found");
        }
        double ivalue = index.getRValue(econtext);
        if (!themap.containsKey(ivalue)) {
            throw new DCExecutionError("Map element not found");
        }
        return themap.get(ivalue);
    }

    @Override
    void assignValue(double new_value, ExecutionContext econtext) {
        Map<Double, Double> themap = econtext.maps.get(base.varname);
        if (themap == null) {
            throw new DCExecutionError("Map not found");
        }
        double ivalue = index.getRValue(econtext);
        themap.put(ivalue, new_value);
    }
}
