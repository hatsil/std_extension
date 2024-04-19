public class ConcurrentTest {
    public static void main(String... args) {
        ConcurrentLinkedDeque<Integer> deque = new ConcurrentLinkedDeque<>();
        deque.add(1);
    }
}