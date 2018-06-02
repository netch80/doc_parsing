import java.util.Map;
import java.util.HashMap;

class ExecutionContext {
    Map<String, Double> scalars = null;
    Map<String, Map<Double, Double>> maps = null;

    ExecutionContext() {
        scalars = new HashMap<>();
        maps = new HashMap<>();
    }

    void createMap(String mapname) {
        maps.put(mapname, new HashMap<>());
    }

}
