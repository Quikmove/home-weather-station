import { db } from "$lib/server/db";
import { home_data } from "$lib/server/db/schema";

type WeatherData = {
    temperature: number;
    humidity: number;
};
export const POST = async ({ request }: { request: Request }) => {
    const data = await request.json();
    const { temperature, humidity } = data as WeatherData;

    if (
        typeof temperature !== "number" ||
        typeof humidity !== "number"
    ) {
        return new Response("Invalid data", { status: 400 });
    }

    const res = await db.insert(home_data).values({
        temperature: temperature,
        humidity: humidity,
    });
    return new Response("Data inserted", { status: 201 });
}
export const GET = async () => {
    // get last one data entry
    const data = await db.select().from(home_data).orderBy(home_data.created).limit(1);
    return new Response(JSON.stringify(data), {
        status: 200,
        headers: { "Content-Type": "application/json" },
    });
}