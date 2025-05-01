import type { HomeData } from "$lib/types/HomeData";
import { error } from "@sveltejs/kit";

export const load = async ({fetch}) => {
    const res = await fetch("/data/weather");
    if (res.status === 200) {
        const data: HomeData[] = await res.json();
        return { homeData:data };
    } else {
        throw error(res.status, "Failed to fetch weather data");
    }
}