
import java.io.File;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.util.List;
import java.util.ArrayList;
import java.util.concurrent.ThreadLocalRandom;

class Node {
    int count;
    private int originalR;
    private int R;
    private int transmissionLeft;
    private int tryTime;
    private int transmissionTime;
    public int success;
    public int collisionCount;

    Node(int R, int L, int M) {
        this.originalR = R;
        this.R = R;
        this.count = ThreadLocalRandom.current().nextInt(R + 1);
        this.tryTime = 0;
        this.transmissionLeft = L;
        this.transmissionTime = L;
        this.success = 0;
        this.collisionCount = 0;
    }

    void doubleR() {
        R *= 2;
        tryTime++;
        if (tryTime == Csda.maxTryTime) {
            tryTime = 0;
            R = originalR;
        }
    }

    void countDown() {
        count--;
    }

    public void resetCount() {
        count = ThreadLocalRandom.current().nextInt(R + 1);
    }


    public void transmit() {
        transmissionLeft--;
        if (transmissionLeft == 0) {
            Csda.channelIdle = true;
            Csda.token = -1;
            transmissionLeft = transmissionTime;
            success++;
            R = originalR;
            resetCount();
            tryTime = 0;
        }
    }

    public void collisionCountAdd() {
        collisionCount++;
    }
}

public class Csda {
    static Node[] nodearray;
    public static boolean channelIdle = true;
    public static int maxTryTime, token;

    private static double variance(double a[], int n) {
        double sum = 0;
        for (int i = 0; i < n; i++)
            sum += a[i];
        double mean = (double) sum / (double) n;
        double sqDiff = 0;
        for (int i = 0; i < n; i++)
            sqDiff += (a[i] - mean) *
                    (a[i] - mean);
        return (double) sqDiff / n;
    }

    public static void main(String[] args) throws IOException {

        FileInputStream inputStream = new FileInputStream(args[0]);
        BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(inputStream));
        String str = null;
        List<Integer> zeroLoc = new ArrayList<>();
        int N = 0, R = 0, L = 0, M = 0, T = 0;
        token = -1;
        int collisionTime = 0;
        int util = 0;
        int idle = 0;
        while ((str = bufferedReader.readLine()) != null) {
            String[] strarr = str.split(" ");
            if (strarr[0].equals("N")) {
                N = Integer.parseInt(strarr[1]);
            } else if (strarr[0].equals("L")) {
                L = Integer.parseInt(strarr[1]);
            } else if (strarr[0].equals("R")) {
                R = Integer.parseInt(strarr[1]);
            } else if (strarr[0].equals("M")) {
                M = Integer.parseInt(strarr[1]);
            } else {
                T = Integer.parseInt(strarr[1]);
            }
        }
        inputStream.close();
        bufferedReader.close();
        int numNode = N;
        maxTryTime = M;
        nodearray = new Node[numNode];
        for (int j = 0; j < nodearray.length; j++) {
            nodearray[j] = new Node(R, L, M);
        }
        for (int i = 0; i < T; i++) {
            if (channelIdle) {
                for (int j = 0; j < nodearray.length; j++) {
                    if (nodearray[j].count == 0) {
                        zeroLoc.add(j);
                    }
                    nodearray[j].countDown();
                }
                if (zeroLoc.size() == 0) {
                    idle++;
                    continue;
                } else if (zeroLoc.size() == 1) {
                    idle++;
                    channelIdle = false;
                    token = zeroLoc.get(0);
                } else {
                    collisionTime++;
                    for (int nodeidx : zeroLoc) {
                        nodearray[nodeidx].doubleR();
                        nodearray[nodeidx].resetCount();
                        nodearray[nodeidx].collisionCountAdd();
                    }
                }
            } else {
                util++;
                nodearray[token].transmit();
            }
            zeroLoc.clear();
        }

        double[] nodeTransmission = new double[numNode];
        double[] nodeCollision = new double[numNode];
        for (int i = 0; i < nodearray.length; i++) {
            nodeTransmission[i] = nodearray[i].success;
            nodeCollision[i] = nodearray[i].collisionCount;
        }
        double varianceTransmission = variance(nodeTransmission, numNode);
        double varianceCollision = variance(nodeCollision, numNode);
        File writename = new File("./output.txt");
        writename.createNewFile();
        BufferedWriter out = new BufferedWriter(new FileWriter(writename));
        out.write(String.format("Channel utilization %f %%\r\n", 100 * (double) util / (double) T));
        out.write(String.format("Channel idle fraction %f %%\r\n", 100 * (double) idle / (double) T));
        out.write(String.format("Total number of collisions %d \r\n", collisionTime));
        out.write(String.format("Variance in number of successful transmissions %f \r\n", varianceTransmission));
        out.write(String.format("Variance in number of collision %f \r\n", varianceCollision));

        out.flush();
        out.close();
    }
}
