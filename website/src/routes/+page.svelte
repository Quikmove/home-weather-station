<script lang="ts">
	import * as Card from "$lib/components/ui/card/";
import type { HomeData } from "$lib/types/HomeData";
export let data;
for(let i=0;i<data.homeData.length;i++)
{
    data.homeData[i].created_at = new Date(data.homeData[i].created_at).toLocaleString();
}

</script>

{#each data.homeData as a}
<div class="card-container">
    <Card.Root>
        <!-- <Card.Header>
            <Card.Title>Room Weather Data</Card.Title>
            <Card.Description>Algirdo g. 31-17</Card.Description>
            </Card.Header> -->
            <Card.Content class="p-4">
                <h2>Temperature</h2>
                <p>{a.temperature}°C</p>
            </Card.Content>
            <Card.Content>
                <h2>Humidity</h2>
                <p>{a.humidity}%</p>
            </Card.Content>
            <Card.Footer>
                <p>Logged at: {a.created_at}</p>
            </Card.Footer>
        </Card.Root>
    </div>

{/each}
<style>
    .card-container {
        display: flex;
        justify-content: center;
        align-items: center;
        margin: 1rem;
    }
</style>